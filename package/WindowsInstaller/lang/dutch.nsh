/*
FuCad Installer Language File
Language: Dutch
*/

!insertmacro LANGFILE_EXT "Dutch"

${LangFileString} TEXT_INSTALL_CURRENTUSER "(Installed for Current User)"

${LangFileString} TEXT_WELCOME "Dit installatie programma zal FuCad op uw systeem installeren.$\r$\n\
				$\r$\n\
				$_CLICK"

#${LangFileString} TEXT_CONFIGURE_PYTHON "Compiling Python scripts..."

${LangFileString} TEXT_FINISH_DESKTOP "Create desktop shortcut"
${LangFileString} TEXT_FINISH_WEBSITE "Visit fucad.org for the latest news, support and tips"

#${LangFileString} FileTypeTitle "FuCad-Document"

#${LangFileString} SecAllUsersTitle "Installeer voor alle gebruikers?"
${LangFileString} SecFileAssocTitle "Bestand associaties"
${LangFileString} SecDesktopTitle "Bureaublad pictogram"

${LangFileString} SecCoreDescription "De FuCad bestanden."
#${LangFileString} SecAllUsersDescription "Installeer FuCad voor alle gebruikers of uitsluitend de huidige gebruiker?"
${LangFileString} SecFileAssocDescription "Associeer het FuCad programma met de .FCStd extensie."
${LangFileString} SecDesktopDescription "Een FuCad pictogram op het Bureaublad."
#${LangFileString} SecDictionaries "Woordenboeken"
#${LangFileString} SecDictionariesDescription "Spell-checker dictionaries that can be downloaded and installed."

#${LangFileString} PathName 'Map met het programma $\"xxx.exe$\"'
#${LangFileString} InvalidFolder '$\"xxx.exe$\" is niet gevonden.'

#${LangFileString} DictionariesFailed 'Download of dictionary for language $\"$R3$\" failed.'

#${LangFileString} ConfigInfo "De volgende configuratie van FuCad zal enige tijd duren."

#${LangFileString} RunConfigureFailed "Mislukte configuratie poging"
${LangFileString} InstallRunning "Het installatieprogramma is al gestart!"
${LangFileString} AlreadyInstalled "FuCad ${APP_SERIES_KEY2} is reeds geinstalleerd!$\r$\n\
				Dou you nevertheles want to install FuCad over the existing version?"
${LangFileString} NewerInstalled "You are trying to install an older version of FuCad than what you have installed.$\r$\n\
				  If you really want this, you must uninstall the existing FuCad $OldVersionNumber before."

#${LangFileString} FinishPageMessage "Gefeliciteerd! FuCad is succesvol geinstalleerd.$\r$\n\
#					$\r$\n\
#					(De eerste keer dat u FuCad start kan dit enige seconden duren.)"
${LangFileString} FinishPageRun "Start FuCad"

${LangFileString} UnNotInRegistryLabel "FuCad is niet gevonden in het Windows register.$\r$\n\
					Snelkoppelingen op het Bureaublad en in het Start Menu worden niet verwijderd."
${LangFileString} UnInstallRunning "U moet FuCad eerst afsluiten!"
${LangFileString} UnNotAdminLabel "U heeft systeem-beheerrechten nodig om FuCad te verwijderen!"
${LangFileString} UnReallyRemoveLabel "Weet u zeker dat u FuCad en alle componenten volledig wil verwijderen van deze computer?"
${LangFileString} UnFuCadPreferencesTitle 'FuCad$\'s user preferences'

#${LangFileString} SecUnProgDescription "Verwijder xxx."
${LangFileString} SecUnPreferencesDescription 'Verwijder FuCad$\'s configuratie map$\r$\n\
						$\"$AppPre\username\$\r$\n\
						$AppSuff\$\r$\n\
						${APP_DIR_USERDATA}$\")$\r$\n\
						voor alle gebruikers.'
${LangFileString} DialogUnPreferences 'You chose to delete the FuCads user configuration.$\r$\n\
						This will also delete all installed FuCad addons.$\r$\n\
						Do you agree with this?'
${LangFileString} SecUnProgramFilesDescription "Verwijder FuCad en alle bijbehorende onderdelen."

${LangFileString} DirNotEmptyWarning "The selected folder '$INSTDIR' is not empty.$\r$\n\
                        The installer will remove all its content before installing. Continue?"
${LangFileString} RMInstDirFailed "Failed to remove '$INSTDIR'.$\r$\n\
                        Make sure you have sufficient permissions and that no files are in use."
