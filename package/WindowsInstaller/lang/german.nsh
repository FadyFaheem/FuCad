/*
FuCad Installer Language File
Language: German
Author: Uwe Stöhr
*/

!insertmacro LANGFILE_EXT "German"

${LangFileString} TEXT_INSTALL_CURRENTUSER "(Installiert für den aktuellen Benutzer)"

${LangFileString} TEXT_WELCOME "Dieser Assistent wird Sie durch die Installation von $(^NameDA) begleiten.$\r$\n\
				$\r$\n\
				$_CLICK"

#${LangFileString} TEXT_CONFIGURE_PYTHON "Kompiliere Python Skripte..."

${LangFileString} TEXT_FINISH_DESKTOP "Ein Symbol auf der Arbeitsoberfläche erzeugen"
${LangFileString} TEXT_FINISH_WEBSITE "Besuchen Sie fucad.org für aktuelle Neuigkeiten"

#${LangFileString} FileTypeTitle "FuCad-Dokument"

#${LangFileString} SecAllUsersTitle "Für alle Nutzer installieren?"
${LangFileString} SecFileAssocTitle "Dateizuordnungen"
${LangFileString} SecDesktopTitle "Desktopsymbol"

${LangFileString} SecCoreDescription "Das Programm FuCad."
#${LangFileString} SecAllUsersDescription "FuCad für alle Nutzer oder nur für den aktuellen Nutzer installieren."
${LangFileString} SecFileAssocDescription "Vernüpfung zwischen FuCad und der .FCStd Dateiendung."
${LangFileString} SecDesktopDescription "Verknüpfung zu FuCad auf dem Desktop."
#${LangFileString} SecDictionaries "Wörterbücher"
#${LangFileString} SecDictionariesDescription "Rechtschreibprüfung- Wörterbucher die heruntergeladen und installiert werden können."

#${LangFileString} PathName 'Pfad zur Datei $\"xxx.exe$\"'
#${LangFileString} InvalidFolder 'Kann die Datei $\"xxx.exe$\" nicht finden.'

#${LangFileString} DictionariesFailed 'Herunterladen des Wörterbuchs für Sprache $\"$R3$\" fehlgeschlagen.'

#${LangFileString} ConfigInfo "Die folgende Konfiguration von FuCad wird eine Weile dauern."

#${LangFileString} RunConfigureFailed "Konnte das Konfigurationsskript nicht ausführen."
${LangFileString} InstallRunning "Der Installer läuft bereits!"
${LangFileString} AlreadyInstalled "FuCad ${APP_SERIES_KEY2} ist bereits installiert!$\r$\n\
				Wollen Sie FuCad dennoch über die bestehende Version installieren?"
${LangFileString} NewerInstalled "Sie versuchen eine Vesion von FuCad zu installieren, die älter als die derzeit installierte ist.$\r$\n\
				  Wenn Sie das wirklich wollen, müssen Sie erst das existierende FuCad $OldVersionNumber deinstallieren."

#${LangFileString} FinishPageMessage "Glückwunsch! FuCad wurde erfolgreich installiert.$\r$\n\
#					$\r$\n\
#					(Der erste Start von FuCad kann etwas länger dauern.)"
${LangFileString} FinishPageRun "FuCad starten"

${LangFileString} UnNotInRegistryLabel "Kann FuCad nicht in der Registry finden.$\r$\n\
					Desktopsymbole und Einträge im Startmenü können nicht entfernt werden."
${LangFileString} UnInstallRunning "Sie müssen FuCad zuerst beenden!"
${LangFileString} UnNotAdminLabel "Sie benötigen Administratorrechte um FuCad zu deinstallieren!"
${LangFileString} UnReallyRemoveLabel "Sind Sie sicher, dass sie FuCad und all seine Komponenten deinstallieren möchten?"
${LangFileString} UnFuCadPreferencesTitle 'FuCads Benutzereinstellungen'

#${LangFileString} SecUnProgDescription "Deinstalliert xxx."
${LangFileString} SecUnPreferencesDescription 'Löscht FuCads Benutzereinstellungen$\r$\n\
						(Ordner $\"$AppPre\username\$\r$\n\
						$AppSuff\$\r$\n\
						${APP_DIR_USERDATA}$\")$\r$\n\
						für Sie oder für alle Benutzer (wenn Sie Admin sind).'
${LangFileString} DialogUnPreferences 'Sie haben ausgewählt, die FuCad-Benutzereinstellungen zu löschen.$\r$\n\
						Dies wird auch alle installierten FuCad-Addons löschen.$\r$\n\
						Sind Sie damit einverstanden?'
${LangFileString} SecUnProgramFilesDescription "Deinstalliert FuCad und all seine Komponenten."

${LangFileString} DirNotEmptyWarning "The selected folder '$INSTDIR' is not empty.$\r$\n\
                        The installer will remove all its content before installing. Continue?"
${LangFileString} RMInstDirFailed "Failed to remove '$INSTDIR'.$\r$\n\
                        Make sure you have sufficient permissions and that no files are in use."
