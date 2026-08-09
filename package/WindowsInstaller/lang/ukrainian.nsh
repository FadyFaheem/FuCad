/*
FuCad Installer Language File
Language: Ukrainian
*/

!insertmacro LANGFILE_EXT "Ukrainian"

${LangFileString} TEXT_INSTALL_CURRENTUSER "(Встановлено для поточного користувача)"

${LangFileString} TEXT_WELCOME "За допомогою цього майстра ви зможете встановити FuCad у вашу систему.$\r$\n\
				$\r$\n\
				$_CLICK"

#${LangFileString} TEXT_CONFIGURE_PYTHON "Обробка скриптів Python..."

${LangFileString} TEXT_FINISH_DESKTOP "Створити значок на стільниці"
${LangFileString} TEXT_FINISH_WEBSITE "Відвідати fucad.org, щоб ознайомитися з новинами, довідковими матеріалами та підказками"

#${LangFileString} FileTypeTitle "Документ FuCad"

#${LangFileString} SecAllUsersTitle "Встановити для всіх користувачів?"
${LangFileString} SecFileAssocTitle "Прив’язка файлів"
${LangFileString} SecDesktopTitle "Піктограма стільниці"

${LangFileString} SecCoreDescription "Файли FuCad."
#${LangFileString} SecAllUsersDescription "Визначає, чи слід встановити FuCad для всіх користувачів, чи лише для поточного користувача."
${LangFileString} SecFileAssocDescription "Файли з суфіксом .FCStd автоматично відкриватимуться за допомогою FuCad."
${LangFileString} SecDesktopDescription "Піктограма FuCad на стільниці."
#${LangFileString} SecDictionaries "Словники"
#${LangFileString} SecDictionariesDescription "Словники для перевірки правопису, які можна отримати і встановити."

#${LangFileString} PathName 'Розташування файла $\"xxx.exe$\"'
#${LangFileString} InvalidFolder 'У вказаній теці немає файла $\"xxx.exe$\".'

#${LangFileString} DictionariesFailed 'Спроба отримання словника для мови $\"$R3$\" зазнала невдачі.'

#${LangFileString} ConfigInfo "Налаштування FuCad може тривати досить довго."

#${LangFileString} RunConfigureFailed "Не вдалося виконати скрипт налаштування"
${LangFileString} InstallRunning "Засіб для встановлення вже працює!"
${LangFileString} AlreadyInstalled "FuCad ${APP_SERIES_KEY2} вже встановлено!$\r$\n\
				Чи хочете ви попри ці зауваження встановити FuCad на місце наявної версії?"
${LangFileString} NewerInstalled "Ви намагаєтеся встановити версію FuCad, яка є застарілою порівняно з вже встановленою.$\r$\n\
				  Якщо ви хочете встановити застарілу версію, вам слід спочатку вилучити вже встановлений FuCad $OldVersionNumber."

#${LangFileString} FinishPageMessage "Вітаємо! FuCad було успішно встановлено.$\r$\n\
#					$\r$\n\
#					(Перший запуск FuCad може тривати декілька секунд.)"
${LangFileString} FinishPageRun "Запустити FuCad"

${LangFileString} UnNotInRegistryLabel "Не вдалося знайти записи FuCad у регістрі.$\r$\n\
					Записи на стільниці і у меню запуску вилучено не буде."
${LangFileString} UnInstallRunning "Спочатку слід завершити роботу програми FuCad!"
${LangFileString} UnNotAdminLabel "Для вилучення FuCad вам слід мати привілеї адміністратора!"
${LangFileString} UnReallyRemoveLabel "Ви справді бажаєте повністю вилучити FuCad і всі його компоненти?"
${LangFileString} UnFuCadPreferencesTitle 'Параметри FuCad, встановлені користувачем'

#${LangFileString} SecUnProgDescription "Вилучає xxx."
${LangFileString} SecUnPreferencesDescription 'Вилучає теку з налаштуваннями FuCad$\r$\n\
						$\"$AppPre\username\$\r$\n\
						$AppSuff\$\r$\n\
						${APP_DIR_USERDATA}$\")$\r$\n\
						для всіх користувачів.'
${LangFileString} DialogUnPreferences 'You chose to delete the FuCads user configuration.$\r$\n\
						This will also delete all installed FuCad addons.$\r$\n\
						Do you agree with this?'
${LangFileString} SecUnProgramFilesDescription "Вилучити FuCad і всі його компоненти."

${LangFileString} DirNotEmptyWarning "The selected folder '$INSTDIR' is not empty.$\r$\n\
                        The installer will remove all its content before installing. Continue?"
${LangFileString} RMInstDirFailed "Failed to remove '$INSTDIR'.$\r$\n\
                        Make sure you have sufficient permissions and that no files are in use."
