/*
FuCad Installer Language File
Language: Russian
*/

!insertmacro LANGFILE_EXT "Russian"

${LangFileString} TEXT_INSTALL_CURRENTUSER "(Установлено для текущего пользователя)"

${LangFileString} TEXT_WELCOME "Этот мастер проведет вас через процесс установки $(^NameDA). $\r$\n\
				$\r$\n\
				$_CLICK"

#${LangFileString} TEXT_CONFIGURE_PYTHON "Компиляция скриптов Python..."

${LangFileString} TEXT_FINISH_DESKTOP "Создать ярлык на рабочем столе"
${LangFileString} TEXT_FINISH_WEBSITE "Перейти на fucad.org за новостями, поддержкой и советами"

#${LangFileString} FileTypeTitle "FuCad-Document"

#${LangFileString} SecAllUsersTitle "Установить для всех пользователей?"
${LangFileString} SecFileAssocTitle "Ассоциации файлов"
${LangFileString} SecDesktopTitle "Значок на рабочем столе"

${LangFileString} SecCoreDescription "Файлы FuCad."
#${LangFileString} SecAllUsersDescription "Установить FuCad для всех пользователей или только для текущего пользователя."
${LangFileString} SecFileAssocDescription "Файлы с расширением .FCStd будут автоматически открываться в FuCad."
${LangFileString} SecDesktopDescription "Значок FuCad на рабочем столе."
#${LangFileString} SecDictionaries "Словари"
#${LangFileString} SecDictionariesDescription "Словари для проверки орфографии, которые можно скачать и установить."

#${LangFileString} PathName 'Путь к файлу $\"xxx.exe$\"'
#${LangFileString} InvalidFolder 'Файл $\"xxx.exe$\" отсутствует по этому пути.'

#${LangFileString} DictionariesFailed 'Не удалось загрузить словарь для языка $\"$R3$\".'

#${LangFileString} ConfigInfo "Следующая конфигурация FuCad займет некоторое время."

#${LangFileString} RunConfigureFailed "Не удалось выполнить сценарий настройки"
${LangFileString} InstallRunning "Установщик уже запущен!"
${LangFileString} AlreadyInstalled "FuCad ${APP_SERIES_KEY2} уже установлен!$\r$\n\
				Вы все равно хотите установить FuCad поверх существующей версии?"
${LangFileString} NewerInstalled "Вы пытаетесь установить более старую версию FuCad, чем уже установленная.$\r$\n\
				  Если вы действительно хотите этого, то сначала необходимо удалить существующий FuCad $OldVersionNumber."

#${LangFileString} FinishPageMessage "Поздравляем! FuCad был успешно установлен.$\r$\n\
#					$\r$\n\
#					(Первый запуск FuCad может занять несколько секунд.)"
${LangFileString} FinishPageRun "Запустить FuCad"

${LangFileString} UnNotInRegistryLabel "Не удалось найти FuCad в реестре.$\r$\n\
					Ярлыки на рабочем столе и в меню Пуск не будут удалены."
${LangFileString} UnInstallRunning "Вы должны сначала закрыть FuCad!"
${LangFileString} UnNotAdminLabel "Необходимо иметь права администратора для удаления FuCad!"
${LangFileString} UnReallyRemoveLabel "Вы действительно хотите полностью удалить FuCad и все его компоненты?"
${LangFileString} UnFuCadPreferencesTitle 'Пользовательские настройки FuCad'

#${LangFileString} SecUnProgDescription "Удалить менеджер xxx."
${LangFileString} SecUnPreferencesDescription 'Удалить настройки FuCad$\r$\n\
						(каталог $\"$AppPre\username\$\r$\n\
						$AppSuff\$\r$\n\
						${APP_DIR_USERDATA}$\")$\r$\n\
						для вас или для всех пользователей (если вы администратор).'
${LangFileString} DialogUnPreferences 'You chose to delete the FuCads user configuration.$\r$\n\
						This will also delete all installed FuCad addons.$\r$\n\
						Do you agree with this?'
${LangFileString} SecUnProgramFilesDescription "Удалить FuCad и все его компоненты."

${LangFileString} DirNotEmptyWarning "The selected folder '$INSTDIR' is not empty.$\r$\n\
                        The installer will remove all its content before installing. Continue?"
${LangFileString} RMInstDirFailed "Failed to remove '$INSTDIR'.$\r$\n\
                        Make sure you have sufficient permissions and that no files are in use."
