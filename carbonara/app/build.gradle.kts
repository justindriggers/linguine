plugins {
    id("com.android.application")
}

android {
    namespace = "com.justindriggers.carbonara"
    compileSdk = 37

    defaultConfig {
        applicationId = "com.justindriggers.carbonara"
        minSdk = 30
        targetSdk = 37
        versionCode = 10
        versionName = "1.0.2"

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
    implementation("androidx.core:core-ktx:1.19.0")
    implementation("androidx.appcompat:appcompat:1.7.1")
    implementation("com.google.android.material:material:1.14.0")
    implementation("androidx.games:games-activity:1.2.2")
    implementation("com.google.android.gms:play-services-games-v2:22.0.0")
    implementation("com.github.sephiroth74:AndroidUIGestureRecognizer:v1.2.7")
}