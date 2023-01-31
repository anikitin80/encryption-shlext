# encryption-shlext
This shell extension dll for Windows explorer uses CryptoAPI and allows to encrypt/decrypt files from explorer context menu.

Installing:

1) register dll using command (as Administrator):
   regsvr32.exe EncryptionShlExt.dll 
2) make logoff/logon or restart explorer.exe

Unistalling:

1) unregister dll using command  (as Administrator):
   regsvr32.exe /u EncryptionShlExt.dll 
2) make logoff/logon or restart explorer.exe
