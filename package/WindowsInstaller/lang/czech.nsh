/*
FuCad Installer Language File
Language: Czech
*/

!insertmacro LANGFILE_EXT "Czech"

${LangFileString} TEXT_INSTALL_CURRENTUSER "(Installed for Current User)"

${LangFileString} TEXT_WELCOME "Tento pomocník vás provede instalací FuCadu.$\r$\n\
				$\r$\n\
				$_CLICK"

#${LangFileString} TEXT_CONFIGURE_PYTHON "Compiling Python scripts..."

${LangFileString} TEXT_FINISH_DESKTOP "Create desktop shortcut"
${LangFileString} TEXT_FINISH_WEBSITE "Visit fucad.org for the latest news, support and tips"

#${LangFileString} FileTypeTitle "FuCad-dokumentů"

#${LangFileString} SecAllUsersTitle "Instalovat pro všechny uživatele?"
${LangFileString} SecFileAssocTitle "Asociovat soubory"
${LangFileString} SecDesktopTitle "Ikonu na plochu"

${LangFileString} SecCoreDescription "Soubory FuCadu."
#${LangFileString} SecAllUsersDescription "Instalovat FuCad pro všechny uživatele nebo pouze pro současného uživatele."
${LangFileString} SecFileAssocDescription "Soubory s příponou .FCStd se automaticky otevřou v FuCadu."
${LangFileString} SecDesktopDescription "Ikonu FuCadu na plochu."
#${LangFileString} SecDictionaries "Slovníky"
#${LangFileString} SecDictionariesDescription "Spell-checker dictionaries that can be downloaded and installed."

#${LangFileString} PathName 'Cesta k souboru $\"xxx.exe$\"'
#${LangFileString} InvalidFolder 'Soubor $\"xxx.exe$\" není v zadané cestě.'

#${LangFileString} DictionariesFailed 'Download of dictionary for language $\"$R3$\" failed.'

#${LangFileString} ConfigInfo "The following configuration of FuCad could take a while."

#${LangFileString} RunConfigureFailed "Nelze spustit konfigurační skript"
${LangFileString} InstallRunning "Instalátor je již spuštěn!"
${LangFileString} AlreadyInstalled "FuCad ${APP_SERIES_KEY2} je již nainstalován!$\r$\n\
				Dou you nevertheles want to install FuCad over the existing version?"
${LangFileString} NewerInstalled "You are trying to install an older version of FuCad than what you have installed.$\r$\n\
				  If you really want this, you must uninstall the existing FuCad $OldVersionNumber before."

#${LangFileString} FinishPageMessage "Blahopřejeme! FuCad byl úspěšně nainstalován.$\r$\n\
#					$\r$\n\
#					(První spuštění FuCadu může trvat delší dobu.)"
${LangFileString} FinishPageRun "Spustit FuCad"

${LangFileString} UnNotInRegistryLabel "Nelze nalézt FuCad v registrech.$\r$\n\
					Zástupce na ploše a ve Start menu nebude smazán."
${LangFileString} UnInstallRunning "Nejprve musíte zavřít FuCad!"
${LangFileString} UnNotAdminLabel "Musíte mít administrátorská práva pro odinstalování FuCadu!"
${LangFileString} UnReallyRemoveLabel "Chcete opravdu smazat FuCad a všechny jeho komponenty?"
${LangFileString} UnFuCadPreferencesTitle 'Uživatelská nastavení FuCadu'

#${LangFileString} SecUnProgDescription "Odinstalovat xxx."
${LangFileString} SecUnPreferencesDescription 'Smazat konfigurační adresář FuCadu$\r$\n\
						$\"$AppPre\username\$\r$\n\
						$AppSuff\$\r$\n\
						${APP_DIR_USERDATA}$\")$\r$\n\
						pro všechny uživatele.'
${LangFileString} DialogUnPreferences 'You chose to delete the FuCads user configuration.$\r$\n\
						This will also delete all installed FuCad addons.$\r$\n\
						Do you agree with this?'
${LangFileString} SecUnProgramFilesDescription "Odinstalovat FuCad a všechny jeho komponenty."

${LangFileString} DirNotEmptyWarning "The selected folder '$INSTDIR' is not empty.$\r$\n\
                        The installer will remove all its content before installing. Continue?"
${LangFileString} RMInstDirFailed "Failed to remove '$INSTDIR'.$\r$\n\
                        Make sure you have sufficient permissions and that no files are in use."
