/*
FuCad Installer Language File
Language: Turkish
*/

!insertmacro LANGFILE_EXT "Turkish"

${LangFileString} TEXT_INSTALL_CURRENTUSER "(Installed for Current User)"

${LangFileString} TEXT_WELCOME "Bu sihirbaz size FuCad programını kuracak.$\r$\n\
				$\r$\n\
				$_CLICK"

#${LangFileString} TEXT_CONFIGURE_PYTHON "Compiling Python scripts..."

${LangFileString} TEXT_FINISH_DESKTOP "Create desktop shortcut"
${LangFileString} TEXT_FINISH_WEBSITE "Visit fucad.org for the latest news, support and tips"

#${LangFileString} FileTypeTitle "FuCad-Document"

#${LangFileString} SecAllUsersTitle "Tüm kullanıcılar için kur?"
${LangFileString} SecFileAssocTitle "Dosya eşleşmeleri"
${LangFileString} SecDesktopTitle "Masaüstü ikonu"

${LangFileString} SecCoreDescription "FuCad dosyaları."
#${LangFileString} SecAllUsersDescription "FuCad tüm kullanıcılar için mi yoksa yalnızca bu kullanıcıya mı kurulacak."
${LangFileString} SecFileAssocDescription "Uzantısı .FCStd olan dosyalar otomatik olarak FuCad ile açılsın."
${LangFileString} SecDesktopDescription "Masaüstüne bir FuCad ikonu koy."
#${LangFileString} SecDictionaries "Sözlükleri"
#${LangFileString} SecDictionariesDescription "Spell-checker dictionaries that can be downloaded and installed."

#${LangFileString} PathName 'Path to the file $\"xxx.exe$\"'
#${LangFileString} InvalidFolder '$\"xxx.exe$\" dosyası belirttiğiniz dizinde bulunamadı.'

#${LangFileString} DictionariesFailed 'Download of dictionary for language $\"$R3$\" failed.'

#${LangFileString} ConfigInfo "Sıradaki FuCad yapılandırması biraz zaman alacak."

#${LangFileString} RunConfigureFailed "Yapılandırma programı çalıştırılamadı"
${LangFileString} InstallRunning "Kurulum programı zaten çalışıyor!"
${LangFileString} AlreadyInstalled "FuCad ${APP_SERIES_KEY2} kurulu zaten!$\r$\n\
				Dou you nevertheles want to install FuCad over the existing version?"
${LangFileString} NewerInstalled "You are trying to install an older version of FuCad than what you have installed.$\r$\n\
				  If you really want this, you must uninstall the existing FuCad $OldVersionNumber before."

#${LangFileString} FinishPageMessage "Tebrikler! FuCad başarıyla kuruldu.$\r$\n\
#					$\r$\n\
#					(FuCad in ilk açılışı birkaç saniye alabilir.)"
${LangFileString} FinishPageRun "FuCad Başlat"

${LangFileString} UnNotInRegistryLabel "Sistem kütüğünde FuCad bulunamadı.$\r$\n\
					Başlat menüsü ve masaüstünüzdeki kısayollar silinemeyecek."
${LangFileString} UnInstallRunning "Önce FuCad i kapatmalısınız!"
${LangFileString} UnNotAdminLabel "FuCad kaldırabilmek için yönetici yetkileri gerekiyor!"
${LangFileString} UnReallyRemoveLabel "FuCad ve tüm bileşenlerini kaldırmak istediğinize emin misiniz?"
${LangFileString} UnFuCadPreferencesTitle 'FuCad$\'s user preferences'

#${LangFileString} SecUnProgDescription "Uninstalls xxx."
${LangFileString} SecUnPreferencesDescription 'Deletes FuCad$\'s configuration folder$\r$\n\
						$\"$AppPre\username\$\r$\n\
						$AppSuff\$\r$\n\
						${APP_DIR_USERDATA}$\")$\r$\n\
						for all users.'
${LangFileString} DialogUnPreferences 'You chose to delete the FuCads user configuration.$\r$\n\
						This will also delete all installed FuCad addons.$\r$\n\
						Do you agree with this?'
${LangFileString} SecUnProgramFilesDescription "Uninstall FuCad and all of its components."

${LangFileString} DirNotEmptyWarning "The selected folder '$INSTDIR' is not empty.$\r$\n\
                        The installer will remove all its content before installing. Continue?"
${LangFileString} RMInstDirFailed "Failed to remove '$INSTDIR'.$\r$\n\
                        Make sure you have sufficient permissions and that no files are in use."
