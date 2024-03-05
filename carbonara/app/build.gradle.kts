plugins {
    id("com.android.application")
    id("org.jetbrains.kotlin.android")
}

android {
    namespace = "com.justindriggers.carbonara"
    compileSdk = 34

    defaultConfig {
        applicationId = "com.justindriggers.carbonara"
        minSdk = 30
        targetSdk = 34
        versionCode = 4
        versionName = "0.0.4"

        ndk {
            debugSymbolLevel = "FULL"
        }
    }

    buildTypes {
        release {
            isMinifyEnabled = false
            proguardFiles(
                getDefaultProguardFile("proguard-android-optimize.txt"),
                "proguard-rules.pro"
            )
            signingConfig = signingConfigs.getByName("debug")
        }
    }
    compileOptions {
        sourceCompatibility = JavaVersion.VERSION_1_8
        targetCompatibility = JavaVersion.VERSION_1_8
    }
    kotlinOptions {
        jvmTarget = "1.8"
    }
    buildFeatures {
        prefab = true
    }
    externalNativeBuild {
        cmake {
            path = file("../../CMakeLists.txt")
            version = "3.22.1"
        }
    }
    sourceSets {
        named("main") {
            assets.srcDirs(
                "../../assets/audio/",
                "../../assets/fonts/"
            )
        }
    }
}

dependencies {
    implementation("androidx.core:core-ktx:1.12.0")
    implementation("androidx.appcompat:appcompat:1.6.1")
    implementation("com.google.android.material:material:1.11.0")
    implementation("androidx.games:games-activity:1.2.2")
    implementation("com.github.sephiroth74:AndroidUIGestureRecognizer:v1.2.7")
}