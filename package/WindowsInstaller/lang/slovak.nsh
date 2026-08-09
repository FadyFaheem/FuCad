/*
FuCad Installer Language File
Language: Slovak
*/

!insertmacro LANGFILE_EXT "Slovak"

${LangFileString} TEXT_INSTALL_CURRENTUSER "(Inštalované pre súčasného užívateľa)"

${LangFileString} TEXT_WELCOME "Tento sprievodca Vám pomáha inštalovať FuCad.$\r$\n\
				$\r$\n\
				$_CLICK"

#${LangFileString} TEXT_CONFIGURE_PYTHON "Kompilácia Python skriptov..."

${LangFileString} TEXT_FINISH_DESKTOP "Vytvoriť skratku pre pracovnú plochu"
${LangFileString} TEXT_FINISH_WEBSITE "Navštívte fucad.org pre posledné novinky, podporu a tipy"

#${LangFileString} FileTypeTitle "FuCad dokument"

#${LangFileString} SecAllUsersTitle "Inštalovať pre všetkých užívateľov?"
${LangFileString} SecFileAssocTitle "Asociácie súborov"
${LangFileString} SecDesktopTitle "Ikona pracovnej plochy"

${LangFileString} SecCoreDescription "Súbory FuCadu."
#${LangFileString} SecAllUsersDescription "Inštalovať FuCad pre všetkých užívateľov alebo len pre súčasného užívateľa."
${LangFileString} SecFileAssocDescription "Súbory s rozšírením .FCStd sa automaticky otvárajú v FuCade."
${LangFileString} SecDesktopDescription "Ikona FuCada na pracovnej ploche."
#${LangFileString} SecDictionaries "Slovníky"
#${LangFileString} SecDictionariesDescription "Slovníky pre kontrolu pravopisu ktoré možno načítať a inštalovať."

#${LangFileString} PathName 'Cesta na súbor $\"xxx.exe$\"'
#${LangFileString} InvalidFolder 'Súbor $\"xxx.exe$\" nie je na špecifikovanej ceste.'

#${LangFileString} DictionariesFailed 'Načítanie slovníka pre jazyk $\"$R3$\" zlyhalo.'

#${LangFileString} ConfigInfo "Nasledujúca konfigurácia FuCadu trochu potrvá."

#${LangFileString} RunConfigureFailed "Nedal sa spustiť konfiguračný skript"
${LangFileString} InstallRunning "Inštalačný program už beží!"
${LangFileString} AlreadyInstalled "FuCad ${APP_SERIES_KEY2} je už inštalovaný!$\r$\n\
				Napriek tomu chcete inštalovať FuCad ponad existujúcu verziu?"
${LangFileString} NewerInstalled "Pokúšate sa inštalovať verziu FuCadu ktorá je staršia ako tá ktorá je inštalovaná.$\r$\n\
				  Keď to naozaj chcete, odinštalujte najprv existujúci FuCad $OldVersionNumber."

#${LangFileString} FinishPageMessage "Gratulácia! FuCad bol úspešne inštalovaný.$\r$\n\
#					$\r$\n\
#					(Prvý FuCad štart môže trvať niekoľko sekúnd.)"
${LangFileString} FinishPageRun "Spustiť FuCad"

${LangFileString} UnNotInRegistryLabel "Nemôžem nájsť FuCad v registre.$\r$\n\
					Skratky na pracovnej ploche a v štartovacom Menu sa nedajú odstrániť."
${LangFileString} UnInstallRunning "Najprv treba zavrieť FuCad!"
${LangFileString} UnNotAdminLabel "Pre odinštaláciu FuCad potrebujete administrátorské práva!"
${LangFileString} UnReallyRemoveLabel "Ste si istý, že chcete kompletne odinštalovať FuCad a všetky jeho súčiastky?"
${LangFileString} UnFuCadPreferencesTitle 'FuCadove užívateľské nastavenia'

#${LangFileString} SecUnProgDescription "Odinštaluje xxx."
${LangFileString} SecUnPreferencesDescription 'Odstráni konfiguračný adresár FuCadu $\r$\n\
						$\"$AppPre\username\$\r$\n\
						$AppSuff\$\r$\n\
						${APP_DIR_USERDATA}$\")$\r$\n\
						pre všetkých užívateľov (keď máte administrátorské práva).'
${LangFileString} DialogUnPreferences 'You chose to delete the FuCads user configuration.$\r$\n\
						This will also delete all installed FuCad addons.$\r$\n\
						Do you agree with this?'
${LangFileString} SecUnProgramFilesDescription "Odinštaluj FuCad a všetky jeho súčiastky."

${LangFileString} DirNotEmptyWarning "The selected folder '$INSTDIR' is not empty.$\r$\n\
                        The installer will remove all its content before installing. Continue?"
${LangFileString} RMInstDirFailed "Failed to remove '$INSTDIR'.$\r$\n\
                        Make sure you have sufficient permissions and that no files are in use."
