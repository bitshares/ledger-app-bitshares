ICONSET_SVG=icon.svg
ICONSET_DIR=build/SGW.iconset
ICONSET_ICON=build/SimpleGUI.png
ICNS_FILE=build/app.icns
ICO_FILE=build/app.ico

UNIX_NAME="simpleguiwallet"
BUNDLE_NAME="SimpleGUIWallet"
VERSION_STRING="0.0.0"


default:

clean:
	rm -rf build

cleanall:
	rm -rf build dist __pycache__

app: $(ICNS_FILE)
	pyinstaller -y build.spec

dmg: dist/$(BUNDLE_NAME).app
	dmgbuild -s dmg_settings.py "$(BUNDLE_NAME) $(VERSION_STRING)" dist/$(UNIX_NAME)-$(VERSION_STRING)-osx.dmg


$(ICONSET_DIR):
	mkdir -p $(ICONSET_DIR)
	convert -background none $(ICONSET_SVG) -resize 1024 $(ICONSET_ICON)
	sips -z 16 16     $(ICONSET_ICON) --out $(ICONSET_DIR)/icon_16x16.png
	sips -z 32 32     $(ICONSET_ICON) --out $(ICONSET_DIR)/icon_16x16@2x.png
	sips -z 32 32     $(ICONSET_ICON) --out $(ICONSET_DIR)/icon_32x32.png
	sips -z 64 64     $(ICONSET_ICON) --out $(ICONSET_DIR)/icon_32x32@2x.png
	sips -z 128 128   $(ICONSET_ICON) --out $(ICONSET_DIR)/icon_128x128.png
	sips -z 256 256   $(ICONSET_ICON) --out $(ICONSET_DIR)/icon_128x128@2x.png
	sips -z 256 256   $(ICONSET_ICON) --out $(ICONSET_DIR)/icon_256x256.png
	sips -z 512 512   $(ICONSET_ICON) --out $(ICONSET_DIR)/icon_256x256@2x.png
	sips -z 512 512   $(ICONSET_ICON) --out $(ICONSET_DIR)/icon_512x512.png
	cp                $(ICONSET_ICON)       $(ICONSET_DIR)/icon_512x512@2x.png

$(ICNS_FILE): $(ICONSET_DIR)
	iconutil -c icns $(ICONSET_DIR) --output $(ICNS_FILE)

icns: $(ICNS_FILE)
	echo "OK"
