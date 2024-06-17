package com.wales

import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.GlobalScope
import kotlinx.coroutines.async
import kotlinx.coroutines.awaitAll
import kotlinx.coroutines.launch
import kotlinx.coroutines.runBlocking
import kotlinx.coroutines.withContext
import smile.data.DataFrame
import smile.data.formula.Formula
import smile.data.vector.DoubleVector
import smile.data.vector.IntVector
import smile.regression.RandomForest
import java.io.File
import java.io.FileInputStream
import java.io.FileOutputStream
import java.io.ObjectInputStream
import java.io.ObjectOutputStream
import java.nio.file.Files
import java.nio.file.Paths
import java.time.LocalDate
import java.time.format.DateTimeFormatter
import kotlin.random.Random

data class FarmElement(val weather: Weather, val yield: Double, val cropType: Crop)
data class Weather(var temp: Double, val sunshine: Double, val precipitation: Double)

enum class Crop {
    RICE,
    PUMPKIN,
    LEAFY,
    NA
}

class WeatherPredictor(private val dataPath: String) {
    private var sunshineModel: RandomForest? = null
    private var tempModel: RandomForest? = null
    private var rainfallModel: RandomForest? = null

    init {
        val modelPath = "models"
        File(modelPath).mkdirs()
        val fullData: DataFrame = readCsv()
        sunshineModel = loadOrCreateModel(
            "$modelPath\\sunshineModel.ser",
            "SunshineDuration ~ Latitude + Longitude + DayOfYear",
            fullData
        )
        tempModel = loadOrCreateModel(
            "$modelPath\\tempModel.ser",
            "MeanTemp ~ Latitude + Longitude + DayOfYear",
            fullData
        )
        rainfallModel = loadOrCreateModel(
            "$modelPath\\rainfallModel.ser",
            "PrecipitationSum ~ Latitude + Longitude + DayOfYear",
            fullData
        )
    }

    private fun loadOrCreateModel(
        fileName: String,
        formulaString: String,
        fullData: DataFrame
    ): RandomForest {
        return if (File(fileName).exists()) {
            println("deserializing")
            deserializeModel(fileName)
        } else {
            val model = RandomForest.fit(Formula.of(formulaString), fullData)
            serializeModel(model, fileName)
            model
        }
    }

    fun serializeModels() {
        sunshineModel?.let { serializeModel(it, "models/sunshineModel.ser") }
        tempModel?.let { serializeModel(it, "models/tempModel.ser") }
        rainfallModel?.let { serializeModel(it, "models/rainfallModel.ser") }
    }

    private fun serializeModel(model: RandomForest, fileName: String) {
        ObjectOutputStream(FileOutputStream(fileName)).use { it.writeObject(model) }
    }

    fun deserializeModel(fileName: String): RandomForest =
        ObjectInputStream(FileInputStream(fileName)).use { it.readObject() as RandomForest }

    fun getWeatherData(latitude: Double, longitude: Double, dayOfYear: Int): Weather? {
        val temp = predictTemperature(latitude, longitude, dayOfYear) ?: return null
        val sunshine = predictSunshine(latitude, longitude, dayOfYear) ?: return null
        val rainfall = predictRainfall(latitude, longitude, dayOfYear) ?: return null
        return Weather(temp, sunshine, rainfall)
    }

    fun predictSunshine(latitude: Double, longitude: Double, dayOfYear: Int): Double? =
        sunshineModel?.predict(
            createInputDataFrame(
                latitude,
                longitude,
                dayOfYear,
                "SunshineDuration"
            )
        )?.get(0)

    fun predictTemperature(latitude: Double, longitude: Double, dayOfYear: Int): Double? =
        tempModel?.predict(createInputDataFrame(latitude, longitude, dayOfYear, "MeanTemp"))?.get(0)

    fun predictRainfall(latitude: Double, longitude: Double, dayOfYear: Int): Double? =
        rainfallModel?.predict(
            createInputDataFrame(
                latitude,
                longitude,
                dayOfYear,
                "PrecipitationSum"
            )
        )?.get(0)

    private fun createInputDataFrame(
        latitude: Double,
        longitude: Double,
        dayOfYear: Int,
        type: String
    ): DataFrame =
        DataFrame.of(
            DoubleVector.of("Latitude", doubleArrayOf(latitude)),
            DoubleVector.of("Longitude", doubleArrayOf(longitude)),
            IntVector.of("DayOfYear", intArrayOf(dayOfYear)),
            DoubleVector.of(type, doubleArrayOf(0.0))
        )

    private fun readCsv(): DataFrame {
        val path = Paths.get(dataPath)
        val lines = Files.readAllLines(path)
        val headers = lines.first().split(",").map(String::trim)

        val dates = IntArray(lines.size - 1)
        val latitudes = DoubleArray(lines.size - 1)
        val longitudes = DoubleArray(lines.size - 1)
        val sunshineDurations = DoubleArray(lines.size - 1)
        val precipitationSums = DoubleArray(lines.size - 1)
        val meanTemps = DoubleArray(lines.size - 1)

        for ((index, line) in lines.drop(1).withIndex()) {
            val tokens = line.split(",").map(String::trim)
            val date = LocalDate.parse(tokens[0], DateTimeFormatter.ISO_LOCAL_DATE)
            dates[index] = date.dayOfYear
            latitudes[index] = tokens[1].toDouble()
            longitudes[index] = tokens[2].toDouble()
            sunshineDurations[index] = tokens[3].toDouble() / 3600 // Convert seconds to hours
            precipitationSums[index] = tokens[4].toDouble()
            meanTemps[index] = tokens[5].toDouble()
        }

        return DataFrame.of(
            IntVector.of("DayOfYear", dates),
            DoubleVector.of("Latitude", latitudes),
            DoubleVector.of("Longitude", longitudes),
            DoubleVector.of("SunshineDuration", sunshineDurations),
            DoubleVector.of("PrecipitationSum", precipitationSums),
            DoubleVector.of("MeanTemp", meanTemps)
        )
    }

    @RequiresApi(Build.VERSION_CODES.O)
    fun evaluateYieldForFarm(
        latitude: Double,
        longitude: Double,
        numRows: Int,
        numCols: Int,
        plantTypes: List<List<Crop>>
    ): List<List<List<FarmElement>>> {
        val farmYieldData = MutableList(numRows) { MutableList(numCols) { MutableList(12) { FarmElement(Weather(0.0, 0.0, 0.0), 0.0, Crop.RICE) } } }

        runBlocking {
            val deferredResults = (0 until numRows).map { row ->
                async(Dispatchers.Default) {
                    processRow(row, numCols, latitude, longitude, plantTypes)
                }
            }

            deferredResults.awaitAll().forEachIndexed { rowIndex, rowData ->
                rowData.forEachIndexed { colIndex, cellData ->
                    cellData.forEachIndexed { monthIndex, farmElement ->
                        farmYieldData[rowIndex][colIndex][monthIndex] = farmElement
                    }
                }
            }
        }

        return farmYieldData
    }

    private suspend fun processRow(
        row: Int,
        numCols: Int,
        latitude: Double,
        longitude: Double,
        plantTypes: List<List<Crop>>
    ): List<List<FarmElement>> = withContext(Dispatchers.Default) {
        val rowData = MutableList(numCols) { col ->
            val cropType = plantTypes[row][col]
            val cellLatitude = latitude + (row * 0.00001)
            val cellLongitude = longitude + (col * 0.00001)

            val cellYieldMap = evaluateYield(cellLatitude, cellLongitude, cropType)
            (0 until 12).map { month ->
                val weatherData = calculateMonthlyWeatherData(cellLatitude, cellLongitude, month + 1)
                FarmElement(weatherData, cellYieldMap["2024-${(month + 1).toString().padStart(2, '0')}"] ?: 0.0, cropType)
            }
        }
        rowData
    }

    private fun calculateMonthlyWeatherData(
        latitude: Double,
        longitude: Double,
        month: Int
    ): Weather {
        val daysInMonth = LocalDate.of(2024, month, 1).lengthOfMonth()
        var totalTemp = 0.0
        var totalSunshine = 0.0
        var totalPrecipitation = 0.0

        for (day in 1..daysInMonth) {
            val dayOfYear = LocalDate.of(2024, month, day).dayOfYear
            val weather = getWeatherData(latitude, longitude, dayOfYear)
            if (weather != null) {
                totalTemp += weather.temp
                totalSunshine += weather.sunshine
                totalPrecipitation += weather.precipitation
            }
        }

        val avgTemp = totalTemp / daysInMonth
        return Weather(avgTemp, totalSunshine, totalPrecipitation)
    }

    fun evaluateYield(latitude: Double, longitude: Double, cropType: Crop): Map<String, Double> {
        val cropCondition = getCropCondition(cropType)
        val yieldMap = mutableMapOf<String, Double>()
        val maxPoints = 21.0

        for (month in 1..12) {
            val weatherList = mutableListOf<Weather>()
            val daysInMonth = LocalDate.of(2024, month, 1).lengthOfMonth()
            var totalPoints = 0.0
            if (cropType != Crop.NA) {
                for (day in 1..daysInMonth) {
                    val dayOfYear = LocalDate.of(2024, month, day).dayOfYear
                    getWeatherData(latitude, longitude, dayOfYear)?.let {
                        weatherList.add(it)
                        totalPoints += calculateDailyPoints(it, cropCondition)
                    }
                }
            }

            if (weatherList.isNotEmpty()) {
                // random +-5% added here due to inherent randomness of crop growth
                val yieldPercentage = ((totalPoints / (weatherList.size * maxPoints)) * 100) + Random.nextDouble(-5.0, 5.0)
                yieldMap["2024-${month.toString().padStart(2, '0')}"] = yieldPercentage
                println("Cell ($latitude, $longitude), Month: $month, Yield: $yieldPercentage%")
            } else {
                println("No weather data available for Cell ($latitude, $longitude), Month: $month")
            }
        }
        return yieldMap
    }

    fun calculateDailyPoints(weather: Weather, cropCondition: CropCondition): Double {
        var points = 0.0
        points += when {
            weather.temp in cropCondition.tempVeryHigh -> 7.0
            weather.temp in cropCondition.tempHigh -> 6.0
            weather.temp in cropCondition.tempModerateHigh -> 5.0
            weather.temp in cropCondition.tempModerate -> 4.0
            weather.temp in cropCondition.tempModerateLow -> 3.0
            weather.temp in cropCondition.tempLow -> 2.0
            else -> 1.0
        }
        points += when {
            weather.sunshine in cropCondition.sunshineVeryHigh -> 7.0
            weather.sunshine in cropCondition.sunshineHigh -> 6.0
            weather.sunshine in cropCondition.sunshineModerateHigh -> 5.0
            weather.sunshine in cropCondition.sunshineModerate -> 4.0
            weather.sunshine in cropCondition.sunshineModerateLow -> 3.0
            weather.sunshine in cropCondition.sunshineLow -> 2.0
            else -> 1.0
        }
        points += when {
            weather.precipitation in cropCondition.precipitationVeryHigh -> 7.0
            weather.precipitation in cropCondition.precipitationHigh -> 6.0
            weather.precipitation in cropCondition.precipitationModerateHigh -> 5.0
            weather.precipitation in cropCondition.precipitationModerate -> 4.0
            weather.precipitation in cropCondition.precipitationModerateLow -> 3.0
            weather.precipitation in cropCondition.precipitationLow -> 2.0
            else -> 1.0
        }

        return points
    }
}

data class CropCondition(
    val tempVeryHigh: ClosedRange<Double>,
    val tempHigh: ClosedRange<Double>,
    val tempModerateHigh: ClosedRange<Double>,
    val tempModerate: ClosedRange<Double>,
    val tempModerateLow: ClosedRange<Double>,
    val tempLow: ClosedRange<Double>,
    val tempVeryLow: ClosedRange<Double>,
    val sunshineVeryHigh: ClosedRange<Double>,
    val sunshineHigh: ClosedRange<Double>,
    val sunshineModerateHigh: ClosedRange<Double>,
    val sunshineModerate: ClosedRange<Double>,
    val sunshineModerateLow: ClosedRange<Double>,
    val sunshineLow: ClosedRange<Double>,
    val sunshineVeryLow: ClosedRange<Double>,
    val precipitationVeryHigh: ClosedRange<Double>,
    val precipitationHigh: ClosedRange<Double>,
    val precipitationModerateHigh: ClosedRange<Double>,
    val precipitationModerate: ClosedRange<Double>,
    val precipitationModerateLow: ClosedRange<Double>,
    val precipitationLow: ClosedRange<Double>,
    val precipitationVeryLow: ClosedRange<Double>
)

private fun getCropCondition(cropType: Crop): CropCondition {
    return when (cropType) {
        Crop.RICE -> CropCondition(
            tempVeryHigh = 28.0..30.0,
            tempHigh = 26.0..28.0,
            tempModerateHigh = 24.0..26.0,
            tempModerate = 22.0..24.0,
            tempModerateLow = 20.0..22.0,
            tempLow = 18.0..20.0,
            tempVeryLow = -Double.MAX_VALUE..18.0,
            sunshineVeryHigh = 240.0..260.0,
            sunshineHigh = 220.0..240.0,
            sunshineModerateHigh = 200.0..220.0,
            sunshineModerate = 180.0..200.0,
            sunshineModerateLow = 160.0..180.0,
            sunshineLow = 140.0..160.0,
            sunshineVeryLow = -Double.MAX_VALUE..140.0,
            precipitationVeryHigh = 280.0..300.0,
            precipitationHigh = 260.0..280.0,
            precipitationModerateHigh = 240.0..260.0,
            precipitationModerate = 220.0..240.0,
            precipitationModerateLow = 200.0..220.0,
            precipitationLow = 180.0..200.0,
            precipitationVeryLow = -Double.MAX_VALUE..180.0
        )
        Crop.PUMPKIN -> CropCondition(
            tempVeryHigh = 30.0..32.0,
            tempHigh = 28.0..30.0,
            tempModerateHigh = 26.0..28.0,
            tempModerate = 24.0..26.0,
            tempModerateLow = 22.0..24.0,
            tempLow = 20.0..22.0,
            tempVeryLow = -Double.MAX_VALUE..20.0,
            sunshineVeryHigh = 280.0..300.0,
            sunshineHigh = 260.0..280.0,
            sunshineModerateHigh = 240.0..260.0,
            sunshineModerate = 220.0..240.0,
            sunshineModerateLow = 200.0..220.0,
            sunshineLow = 180.0..200.0,
            sunshineVeryLow = -Double.MAX_VALUE..180.0,
            precipitationVeryHigh = 80.0..100.0,
            precipitationHigh = 70.0..80.0,
            precipitationModerateHigh = 60.0..70.0,
            precipitationModerate = 50.0..60.0,
            precipitationModerateLow = 40.0..50.0,
            precipitationLow = 30.0..40.0,
            precipitationVeryLow = -Double.MAX_VALUE..30.0
        )
        Crop.LEAFY -> CropCondition(
            tempVeryHigh = 22.0..24.0,
            tempHigh = 20.0..22.0,
            tempModerateHigh = 18.0..20.0,
            tempModerate = 16.0..18.0,
            tempModerateLow = 14.0..16.0,
            tempLow = 12.0..14.0,
            tempVeryLow = -Double.MAX_VALUE..12.0,
            sunshineVeryHigh = 200.0..220.0,
            sunshineHigh = 180.0..200.0,
            sunshineModerateHigh = 160.0..180.0,
            sunshineModerate = 140.0..160.0,
            sunshineModerateLow = 120.0..140.0,
            sunshineLow = 100.0..120.0,
            sunshineVeryLow = -Double.MAX_VALUE..100.0,
            precipitationVeryHigh = 70.0..80.0,
            precipitationHigh = 60.0..70.0,
            precipitationModerateHigh = 50.0..60.0,
            precipitationModerate = 40.0..50.0,
            precipitationModerateLow = 30.0..40.0,
            precipitationLow = 20.0..30.0,
            precipitationVeryLow = -Double.MAX_VALUE..20.0
        )

        Crop.NA -> CropCondition(
            tempVeryHigh = 0.0..0.0,
            tempHigh = 0.0..0.0,
            tempModerateHigh = 0.0..0.0,
            tempModerate = 0.0..0.0,
            tempModerateLow = 0.0..0.0,
            tempLow = 0.0..0.0,
            tempVeryLow = 0.0..0.0,
            sunshineVeryHigh = 0.0..0.0,
            sunshineHigh = 0.0..0.0,
            sunshineModerateHigh = 0.0..0.0,
            sunshineModerate = 0.0..0.0,
            sunshineModerateLow = 0.0..0.0,
            sunshineLow = 0.0..0.0,
            sunshineVeryLow = 0.0..0.0,
            precipitationVeryHigh = 0.0..0.0,
            precipitationHigh = 0.0..0.0,
            precipitationModerateHigh = 0.0..0.0,
            precipitationModerate = 0.0..0.0,
            precipitationModerateLow = 0.0..0.0,
            precipitationLow = 0.0..0.0,
            precipitationVeryLow = 0.0..0.0
        )
    }
}
