# -*- mode: python ; coding: utf-8 -*-

block_cipher = None


a = Analysis(['SimpleGUIWallet.py'],
             pathex=['/Users/chris/Desktop/untitled folder 2/ledger-app-bitshares/SimpleGUIWallet'],
             binaries=[],
             datas=[],
             hiddenimports=[],
             hookspath=[],
             runtime_hooks=[],
             excludes=[],
             win_no_prefer_redirects=False,
             win_private_assemblies=False,
             cipher=block_cipher,
             noarchive=False)
pyz = PYZ(a.pure, a.zipped_data,
             cipher=block_cipher)
exe = EXE(pyz,
          a.scripts,
          [],
          exclude_binaries=True,
          name='SimpleGUIWallet',
          debug=False,
          bootloader_ignore_signals=False,
          strip=False,
          upx=True,
          console=False )
coll = COLLECT(exe,
               a.binaries,
               a.zipfiles,
               a.datas,
               strip=False,
               upx=True,
               upx_exclude=[],
               name='SimpleGUIWallet')
app = BUNDLE(coll,
             name='SimpleGUIWallet.app',
             icon='build/app.icns',
             bundle_identifier=None,
             info_plist={
                 "NSHighResolutionCapable": True,
             }
            )
