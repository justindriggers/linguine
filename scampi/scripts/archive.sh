#!/bin/sh

PRODUCT_BUNDLE_IDENTIFIER="$1"
DEVELOPMENT_TEAM="$2"
PROVISIONING_PROFILE_SPECIFIER="$3"

cat <<EOF > exportOptions.plist
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">
<plist version="1.0">
<dict>
    <key>method</key>
    <string>app-store</string>
	<key>teamID</key>
	<string>$DEVELOPMENT_TEAM</string>
	<key>uploadSymbols</key>
	<true/>
	<key>signingStyle</key>
	<string>manual</string>
	<key>provisioningProfiles</key>
	<dict>
		<key>$PRODUCT_BUNDLE_IDENTIFIER</key>
		<string>$PROVISIONING_PROFILE_SPECIFIER</string>
	</dict>
</dict>
</plist>
EOF

xcodebuild -workspace scampi.xcodeproj/project.xcworkspace archive -scheme scampi -sdk iphoneos -configuration Release -archivePath scampi.xcarchive
xcodebuild -exportArchive -archivePath scampi.xcarchive -exportPath . -exportOptionsPlist exportOptions.plist

echo "$ALTOOL_PASSWORD" | xcrun altool --validate-app -t ios -f scampi.ipa -u "$ALTOOL_USERNAME"
