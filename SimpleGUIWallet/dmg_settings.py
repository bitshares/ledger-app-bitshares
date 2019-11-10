BUNDLE_NAME="SimpleGUIWallet"

APP=BUNDLE_NAME+".app"

files = [ 'dist/' + APP ]
symlinks = { 'Applications': '/Applications' }

icon = 'dist/' + APP + '/Contents/Resources/app.icns'
badge_icon = 'dist/' + APP + '/Contents/Resources/app.icns'
icon_locations = {
               APP: (100, 100),
    'Applications': (500, 100)
}

background = 'builtin-arrow'
