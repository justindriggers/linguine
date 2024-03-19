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
        versionCode = 8
        versionName = "1.0.0"

        ndk {
            debugSymbolLevel = "FULL"
        }
    }

    signingConfigs {
        create("release") {
            storeFile = file(project.properties["release.storeFile"].toString())
            storePassword = project.properties["release.storePassword"].toString()
            keyAlias = project.properties["release.keyAlias"].toString()
            keyPassword = project.properties["release.keyPassword"].toString()
        }
    }

    buildTypes {
        release {
            isMinifyEnabled = false
            proguardFiles(
                getDefaultProguardFile("proguard-android-optimize.txt"),
                "proguard-rules.pro"
            )
            signingConfig = signingConfigs.getByName("release")
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
    implementation("com.google.android.gms:play-services-games-v2:19.0.0")
    implementation("com.github.sephiroth74:AndroidUIGestureRecognizer:v1.2.7")
}