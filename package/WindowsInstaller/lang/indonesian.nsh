/*
FuCad Installer Language File
Language: Indonesian
*/

!insertmacro LANGFILE_EXT "Indonesian"

${LangFileString} TEXT_INSTALL_CURRENTUSER "(Pasang untuk Pengguna Yang Ini)"

${LangFileString} TEXT_WELCOME "Program ini akan memandu anda dalam melakukan instalasi FuCad.$\r$\n\
				$\r$\n\
				$_CLICK"

#${LangFileString} TEXT_CONFIGURE_PYTHON "Proses kompilasi skrip Python ..."

${LangFileString} TEXT_FINISH_DESKTOP "Membuat pintasan ikon di destop"
${LangFileString} TEXT_FINISH_WEBSITE "Kunjungi fucad.org untuk berita terbaru serta dukungan"

#${LangFileString} FileTypeTitle "Dokumen-FuCad"

#${LangFileString} SecAllUsersTitle "Pasang untuk semua pengguna?"
${LangFileString} SecFileAssocTitle "Berkas yang terkait"
${LangFileString} SecDesktopTitle "Ikon Desktop"

${LangFileString} SecCoreDescription "Berkas-berkas FuCad."
#${LangFileString} SecAllUsersDescription "Pasang FuCad untuk semua pengguna atau hanya untuk pengguna ini saja."
${LangFileString} SecFileAssocDescription "Berkas dengan ekstensi .FCStd akan otomatis dibuka menggunakan FuCad."
${LangFileString} SecDesktopDescription "Ikon FuCad muncul di desktop."
#${LangFileString} SecDictionaries "Kamus"
#${LangFileString} SecDictionariesDescription "Kamus pemeriksa ejaan yang dapat diunduh dan dipasang."

#${LangFileString} PathName 'Lokasi tempat berkas $\"xxx.exe$\" berada'
#${LangFileString} InvalidFolder 'Berkas $\"xxx.exe$\" tidak berada di lokasi tempat yang dinyatakan.'

#${LangFileString} DictionariesFailed 'Download of dictionary for language $\"$R3$\" failed.'

#${LangFileString} ConfigInfo "Proses konfigurasi FuCad selanjutnya akan memerlukan waktu beberapa saat."

#${LangFileString} RunConfigureFailed "Tidak bisa menjalankan skrip konfigurasi"
${LangFileString} InstallRunning "Program instalasi sedang berjalan!"
${LangFileString} AlreadyInstalled "FuCad ${APP_SERIES_KEY2} sudah pernah diinstal!$\r$\n\
				Dou you nevertheles want to install FuCad over the existing version?"
${LangFileString} NewerInstalled "Anda akan memasang versi FuCad yang lama dari yang sudah terpasang.$\r$\n\
				  Jika memang dikehendaki, anda harus menghapus dulu FuCad yang ada, FuCad $OldVersionNumber."

#${LangFileString} FinishPageMessage "Selamat! FuCad berhasil diinstal dengan sukses.$\r$\n\
#					$\r$\n\
#					(Menjalankan FuCad untuk pertama kali memerlukan waktu beberapa detik.)"
${LangFileString} FinishPageRun "Menjalankan FuCad"

${LangFileString} UnNotInRegistryLabel "Tidak bisa menemukan FuCad di catatan registry.$\r$\n\
					Shortcuts pada desktop dan yang ada di Menu Start tidak dihapus."
${LangFileString} UnInstallRunning "Anda harus menutup FuCad terlebih dahulu!"
${LangFileString} UnNotAdminLabel "Anda harus berlaku dan mempunyai hak sebagai administrator untuk menghapus FuCad!"
${LangFileString} UnReallyRemoveLabel "Apakah anda yakin akan menghapus FuCad secara menyeluruh termasuk semua komponen yang ada?"
${LangFileString} UnFuCadPreferencesTitle 'Preferensi pengguna FuCad'

#${LangFileString} SecUnProgDescription "Penghapusan program xxx."
${LangFileString} SecUnPreferencesDescription 'Menghapus berkas konfigurasi FuCad pada folder$\r$\n\
						$\"$AppPre\username\$\r$\n\
						$AppSuff\$\r$\n\
						${APP_DIR_USERDATA}$\")$\r$\n\
						untuk semua pengguna.'
${LangFileString} DialogUnPreferences 'You chose to delete the FuCads user configuration.$\r$\n\
						This will also delete all installed FuCad addons.$\r$\n\
						Do you agree with this?'
${LangFileString} SecUnProgramFilesDescription "Penghapusan FuCad serta semua komponen yang ada."

${LangFileString} DirNotEmptyWarning "The selected folder '$INSTDIR' is not empty.$\r$\n\
                        The installer will remove all its content before installing. Continue?"
${LangFileString} RMInstDirFailed "Failed to remove '$INSTDIR'.$\r$\n\
                        Make sure you have sufficient permissions and that no files are in use."
