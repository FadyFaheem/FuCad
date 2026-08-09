/*
FuCad Installer Language File
Language: Basque
*/

!insertmacro LANGFILE_EXT "Basque"

${LangFileString} TEXT_INSTALL_CURRENTUSER "(Instalatu uneko erabiltzailearentzat)"

${LangFileString} TEXT_WELCOME "Morroi honek $(^NameDA) aplikazioaren instalazio urratsetan zehar lagunduko dizu, $\r$\n\
				$\r$\n\
				$_CLICK"

#${LangFileString} TEXT_CONFIGURE_PYTHON "Python script-ak konpilatzen..."

${LangFileString} TEXT_FINISH_DESKTOP "Sortu mahaigaineko lasterbidea"
${LangFileString} TEXT_FINISH_WEBSITE "Bisitatu fucad.org azken berriak, aholkuak eta laguntza lortzeko"

#${LangFileString} FileTypeTitle "FuCad-dokumentua"

#${LangFileString} SecAllUsersTitle "Instalatu erabiltzaile guztientzako?"
${LangFileString} SecFileAssocTitle "Fitxategiaren esleipenak"
${LangFileString} SecDesktopTitle "Mahaigaineko ikonoa"

${LangFileString} SecCoreDescription "FuCad fitxategiak."
#${LangFileString} SecAllUsersDescription "Instalatu FuCad erabiltzaile guztientzako, edo soilik uneko erabiltzailearentzako."
${LangFileString} SecFileAssocDescription ".FCStd luzapeneko fitxategiak FuCad-ekin irekiko dira automatikoki."
${LangFileString} SecDesktopDescription "FuCad ikonoa mahaigainean."
#${LangFileString} SecDictionaries "Hiztegia"
#${LangFileString} SecDictionariesDescription "Zuzentzaile ortografikoen hiztegiak deskarga eta instala daitezke."

#${LangFileString} PathName '$\"xxx.exe$\" fitxategiaren bide-izena'
#${LangFileString} InvalidFolder '$\"xxx.exe$\" fitxategia ez dago zehaztutako bide-izenean.'

#${LangFileString} DictionariesFailed 'Huts egin du  $\"$R3$\" hizkuntzaren hiztegia deskargatzean.'

#${LangFileString} ConfigInfo "FuCad-en hurrengo konfigurazioak denbora piskat beharko du."

#${LangFileString} RunConfigureFailed "Ezin izan da konfigurazioaren script-a exekutatu"
${LangFileString} InstallRunning "Instalatzailea jadanik exekutatzen ari da."
${LangFileString} AlreadyInstalled "FuCad ${APP_SERIES_KEY2} jadanik instalatuta dago!$\r$\n\
				Dou you nevertheles want to install FuCad over the existing version?"
${LangFileString} NewerInstalled "Instalatuta dagoen FuCad baino bertsio zaharragoa instalatzen saiatzen ari zara.$\r$\n\
				  Hori egitea nahi baduzu, lehenbizi existitzen den FuCad $OldVersionNumber desinstalatu beharko duzu."

#${LangFileString} FinishPageMessage "Zorionak! FuCad ongi instalatu da.$\r$\n\
#					$\r$\n\
#					(FuCad aurreneko aldiz abiatzean denbora piskat beharko du.)"
${LangFileString} FinishPageRun "Abiarazi FuCad"

${LangFileString} UnNotInRegistryLabel "Ezin da FuCad aurkitu erregistroan.$\r$\n\
					Mahaigaineko eta Hasiera menuko lasterbideak ez dira kenduko."
${LangFileString} UnInstallRunning "Aurrenik FuCad itxi behar duzu."
${LangFileString} UnNotAdminLabel "Administratzailearen baimenak behar dituzu FuCad desinstalatzeko."
${LangFileString} UnReallyRemoveLabel "Ziur zaude FuCad eta bere osagai guztiak kentzea nahi dituzula??"
${LangFileString} UnFuCadPreferencesTitle 'FuCad-eko erabiltzailearen hobespenak'

#${LangFileString} SecUnProgDescription "xxx kudeatzailea desinstalatzen du."
${LangFileString} SecUnPreferencesDescription 'FuCad-en konfigurazioa ezabatzen du$\r$\n\
						($\"$AppPre\erabiltzailea\$\r$\n\
						$AppSuff\$\r$\n\
						\${APP_DIR_USERDATA}$\"$\r$\n\
						zuretzako edo erabiltzaile guztientzako (administratzailea bazara).'
${LangFileString} DialogUnPreferences 'You chose to delete the FuCads user configuration.$\r$\n\
						This will also delete all installed FuCad addons.$\r$\n\
						Do you agree with this?'
${LangFileString} SecUnProgramFilesDescription "Desinstalatu FuCad eta bere osagai guztiak."

${LangFileString} DirNotEmptyWarning "The selected folder '$INSTDIR' is not empty.$\r$\n\
                        The installer will remove all its content before installing. Continue?"
${LangFileString} RMInstDirFailed "Failed to remove '$INSTDIR'.$\r$\n\
                        Make sure you have sufficient permissions and that no files are in use."
