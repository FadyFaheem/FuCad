/*
FuCad Installer Language File
Language: Norwegian
*/

!insertmacro LANGFILE_EXT "Norwegian"

${LangFileString} TEXT_INSTALL_CURRENTUSER "(Installer for denne brukeren)"

${LangFileString} TEXT_WELCOME "Denne veiviseren installerer FuCad på datamaskinen din.$\r$\n\
				$\r$\n\
				$_CLICK"

#${LangFileString} TEXT_CONFIGURE_PYTHON "Kompilerer Python script..."

${LangFileString} TEXT_FINISH_DESKTOP "Lager snarveg på skrivebordet"
${LangFileString} TEXT_FINISH_WEBSITE "Besøk fucad.org for de seneste nyhetene, hjelp og støtte"

#${LangFileString} FileTypeTitle "FuCad-dokument"

#${LangFileString} SecAllUsersTitle "Installer for alle brukere?"
${LangFileString} SecFileAssocTitle "Fil-assosiasjoner"
${LangFileString} SecDesktopTitle "Skrivebordsikon"

${LangFileString} SecCoreDescription "FuCad-filene."
#${LangFileString} SecAllUsersDescription "Installer FuCad for alle brukere, eller kun for denne brukeren."
${LangFileString} SecFileAssocDescription "Filer med endelsen .FCStd åpnes automatisk i FuCad."
${LangFileString} SecDesktopDescription "Et FuCad-ikon på skrivebordet."
#${LangFileString} SecDictionaries "Ordbøker"
#${LangFileString} SecDictionariesDescription "Ordbøker til rettskrivningsprogram som kan lastes ned og installeres."

#${LangFileString} PathName 'Stien til filen $\"xxx.exe$\"'
#${LangFileString} InvalidFolder 'Filen $\"xxx.exe$\" fins ikke i den oppgitte mappa.'

#${LangFileString} DictionariesFailed 'Nedlastingen av ordliste for språket $\"$R3$\" feilet.'

#${LangFileString} ConfigInfo "Konfigurasjon av FuCad vil ta en stund."

#${LangFileString} RunConfigureFailed "Fikk ikke kjørt konfigurasjonsscriptet"
${LangFileString} InstallRunning "Installasjonsprogrammet er allerede i gang!"
${LangFileString} AlreadyInstalled "FuCad ${APP_SERIES_KEY2} er allerede installert!$\r$\n\
				Vil du likevel installere FuCad over den eksisterende versjonen?"
${LangFileString} NewerInstalled "Du prøver å installere en eldre versjon av FuCad enn den du har installert fra før.$\r$\n\
				  Dersom du ønsker dette må du avinstallere FuCad $OldVersionNumber først."

#${LangFileString} FinishPageMessage "Gratulerer!! FuCad er installert.$\r$\n\
#					$\r$\n\
#					(Første gangs oppstart av FuCad kan ta noen sekunder.)"
${LangFileString} FinishPageRun "Start FuCad"

${LangFileString} UnNotInRegistryLabel "Fant ikke FuCad i registeret.$\r$\n\
					Snarveier på skrivebordet og i startmenyen fjernes ikke."
${LangFileString} UnInstallRunning "Du må avslutte FuCad først!"
${LangFileString} UnNotAdminLabel "Du må ha administratorrettigheter for å fjerne FuCad!"
${LangFileString} UnReallyRemoveLabel "Er du sikker på at du vil fjerne FuCad og alle tilhørende komponenter?"
${LangFileString} UnFuCadPreferencesTitle 'FuCad sine bruker innstillinger'

#${LangFileString} SecUnProgDescription "Avinstallerer xxx."
${LangFileString} SecUnPreferencesDescription 'Sletter FuCad sine konfigurasjonsmapper$\r$\n\
						$\"$AppPre\username\$\r$\n\
						$AppSuff\$\r$\n\
						${APP_DIR_USERDATA}$\")$\r$\n\
						for alle brukere.'
${LangFileString} DialogUnPreferences 'You chose to delete the FuCads user configuration.$\r$\n\
						This will also delete all installed FuCad addons.$\r$\n\
						Do you agree with this?'
${LangFileString} SecUnProgramFilesDescription "Avinstallerer FuCad og alle delkomponenter."

${LangFileString} DirNotEmptyWarning "The selected folder '$INSTDIR' is not empty.$\r$\n\
                        The installer will remove all its content before installing. Continue?"
${LangFileString} RMInstDirFailed "Failed to remove '$INSTDIR'.$\r$\n\
                        Make sure you have sufficient permissions and that no files are in use."
