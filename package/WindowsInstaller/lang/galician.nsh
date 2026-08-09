/*
FuCad Installer Language File
Language: Galician
*/

!insertmacro LANGFILE_EXT "Galician"

${LangFileString} TEXT_INSTALL_CURRENTUSER "(Installed for Current User)"

${LangFileString} TEXT_WELCOME "Este asistente vai-no guiar na instalación do FuCad no seu computador.$\r$\n\
				$\r$\n\
				$_CLICK"

#${LangFileString} TEXT_CONFIGURE_PYTHON "Compiling Python scripts..."

${LangFileString} TEXT_FINISH_DESKTOP "Create desktop shortcut"
${LangFileString} TEXT_FINISH_WEBSITE "Visit fucad.org for the latest news, support and tips"

#${LangFileString} FileTypeTitle "Documento FuCad"

#${LangFileString} SecAllUsersTitle "Instalar para todos os usuários?"
${LangFileString} SecFileAssocTitle "Asociación dos ficheiros"
${LangFileString} SecDesktopTitle "Icone do ambiente de traballo"

${LangFileString} SecCoreDescription "Os ficheiros FuCad."
#${LangFileString} SecAllUsersDescription "Instalar o FuCad monousuário ou multiusuário."
${LangFileString} SecFileAssocDescription "Asociar a extensión .FCStd co FuCad."
${LangFileString} SecDesktopDescription "Cria un icone do FuCad no ambiente de traballo."
#${LangFileString} SecDictionaries "Dicionarios"
#${LangFileString} SecDictionariesDescription "Spell-checker dictionaries that can be downloaded and installed."

#${LangFileString} PathName 'Camiño ao ficheiro $\"xxx.exe$\"'
#${LangFileString} InvalidFolder 'O ficheiro $\"xxx.exe$\" non está no camiño especificado.'

#${LangFileString} DictionariesFailed 'Download of dictionary for language $\"$R3$\" failed.'

#${LangFileString} ConfigInfo "A configuración seguinte do FuCad irá demorar un pouco."

#${LangFileString} RunConfigureFailed "Non se deu executado o script de configuración"
${LangFileString} InstallRunning "O instalador xa está a correr!"
${LangFileString} AlreadyInstalled "O FuCad ${APP_SERIES_KEY2} xa está instalado!$\r$\n\
				Dou you nevertheles want to install FuCad over the existing version?"
${LangFileString} NewerInstalled "You are trying to install an older version of FuCad than what you have installed.$\r$\n\
				  If you really want this, you must uninstall the existing FuCad $OldVersionNumber before."

#${LangFileString} FinishPageMessage "Parabéns! O FuCad foi instalado con suceso.$\r$\n\
#					$\r$\n\
#					(O primeiro início do FuCad pode levar alguns segundos.)"
${LangFileString} FinishPageRun "Lanzar o FuCad"

${LangFileString} UnNotInRegistryLabel "Non se da achado o FuCad no registo.$\r$\n\
					Non se eliminarán os atallos para o ambiente de traballo e no menu de Início."
${LangFileString} UnInstallRunning "Debe fechar o FuCad en primeiro lugar!"
${LangFileString} UnNotAdminLabel "Precisa de priviléxios de administrador para desinstalar o FuCad!"
${LangFileString} UnReallyRemoveLabel "Seguro que quer eliminar completamente o FuCad e todos os seus componentes?"
${LangFileString} UnFuCadPreferencesTitle 'Preferéncias de usuário do FuCad'

#${LangFileString} SecUnProgDescription "Desinstala xxx."
${LangFileString} SecUnPreferencesDescription 'Elimina as pastas de configuración do  FuCad$\r$\n\
						$\"$AppPre\username\$\r$\n\
						$AppSuff\$\r$\n\
						${APP_DIR_USERDATA}$\")$\r$\n\
						de todos os usuários.'
${LangFileString} DialogUnPreferences 'You chose to delete the FuCads user configuration.$\r$\n\
						This will also delete all installed FuCad addons.$\r$\n\
						Do you agree with this?'
${LangFileString} SecUnProgramFilesDescription "Desinstala FuCad e todos os seus componentes."

${LangFileString} DirNotEmptyWarning "The selected folder '$INSTDIR' is not empty.$\r$\n\
                        The installer will remove all its content before installing. Continue?"
${LangFileString} RMInstDirFailed "Failed to remove '$INSTDIR'.$\r$\n\
                        Make sure you have sufficient permissions and that no files are in use."
