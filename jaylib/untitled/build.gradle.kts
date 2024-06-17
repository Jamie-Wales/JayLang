plugins {
    alias(plugins.android.application)
    alias(libs.plugins.jetbrains.kotlin.android)
    id("com.google.android.libraries.mapsplatform.secrets-gradle-plugin")
    id("kotlin-kapt")
    id("com.google.dagger.hilt.android")
    kotlin("plugin.serialization") version "1.8.0"
}

dependencies {
    // serialization
    implementation(libs.androidx.core.ktx)
    implementation(libs.androidx.lifecycle.runtime.ktx)
    implementation(libs.androidx.activity.compose)
    implementation(platform(libs.androidx.compose.bom))
    implementation(libs.androidx.ui)
    implementation(libs.androidx.ui.graphics)
    implementation(libs.androidx.ui.tooling.preview)
    implementation(libs.androidx.material3)
    implementation(libs.androidx.ui.text.google.fonts)
    implementation(libs.retrofit)
    implementation(libs.converter.gson)
    implementation(libs.androidx.navigation.compose)
    implementation(libs.androidx.appcompat)
    implementation(libs.androidx.navigation.testing)
    implementation(libs.smile.kotlin)
    testImplementation(libs.junit)

    implementation(libs.kotlinx.serialization.json)

    implementation(libs.androidx.datastore.preferences)

    implementation(libs.android.maps.compose)
    implementation(libs.maps.compose.utils)
    implementation(libs.maps.compose.widgets)

    implementation("com.esotericsoftware:kryo:5.3.0")

    androidTestImplementation(libs.androidx.junit)
    androidTestImplementation(libs.androidx.espresso.core)
    androidTestImplementation(platform(libs.androidx.compose.bom))
    androidTestImplementation(libs.androidx.ui.test.junit4)
    debugImplementation(libs.androidx.ui.tooling)
    debugImplementation(libs.androidx.ui.test.manifest)

    implementation(libs.hilt.android)
    kapt(libs.hilt.android.compiler)

    implementation(libs.play.services.location)
    implementation(libs.accompanist.permissions.v0350alpha)
}
