/*
FuCad Installer Language File
Language: Brazilian Portuguese
*/

!insertmacro LANGFILE_EXT "PortugueseBR"

${LangFileString} TEXT_INSTALL_CURRENTUSER "(Instalado para o Usuário Atual)"

${LangFileString} TEXT_WELCOME "Este assistente guiará você durante a instalação do $(^NameDA), $\r$\n\
				$\r$\n\
				$_CLICK"

#${LangFileString} TEXT_CONFIGURE_PYTHON "Compilando scripts Python..."

${LangFileString} TEXT_FINISH_DESKTOP "Criar atalho na área de trabalho"
${LangFileString} TEXT_FINISH_WEBSITE "Visite fucad.org para ver as últimas novidades do FuCad!"

#${LangFileString} FileTypeTitle "Documento-FuCad"

#${LangFileString} SecAllUsersTitle "Instalar para todos os usuários?"
${LangFileString} SecFileAssocTitle "Associações de arquivos"
${LangFileString} SecDesktopTitle "Ícone de área de trabalho"

${LangFileString} SecCoreDescription "Os arquivos do FuCad."
#${LangFileString} SecAllUsersDescription "Instalar o FuCad para todos os usuários ou apenas para o usuário atual."
${LangFileString} SecFileAssocDescription "Arquivos com a extensão .FCStd serão abertos automaticamente no FuCad."
${LangFileString} SecDesktopDescription "Um ícone do FuCad na área de trabalho."
#${LangFileString} SecDictionaries "Dicionários"
#${LangFileString} SecDictionariesDescription "Dicionários ortográficos que podem ser baixados e instalados."

#${LangFileString} PathName 'Caminho para o arquivo $\"xxx.exe$\"'
#${LangFileString} InvalidFolder 'O arquivo $\"xxx.exe$\" não existe no caminho especificado.'

#${LangFileString} DictionariesFailed 'Ocorreu uma falha ao baixar o dicionário ortográfico do idioma $\"$R3$\".'

#${LangFileString} ConfigInfo "A configuração do FuCad que será feita a seguir vai demorar bastante."

#${LangFileString} RunConfigureFailed "Não foi possível executar o script de configuração"
${LangFileString} InstallRunning "O instalador já está em execução!"
${LangFileString} AlreadyInstalled "O FuCad ${APP_SERIES_KEY2} já está instalado!$\r$\n\
				Deseja instalar sobre a versão existente mesmo assim?"
${LangFileString} NewerInstalled "A versão que você está tentando instalar é mais antiga que aquela que já está instalada.$\r$\n\
				  Se isso for realmente o que deseja, primeiro desinstale o FuCad $OldVersionNumber."

#${LangFileString} FinishPageMessage "Parabéns! O FuCad foi instalado com sucesso.$\r$\n\
#					$\r$\n\
#					(A primeira execução do FuCad pode demorar alguns segundos.)"
${LangFileString} FinishPageRun "Executar o FuCad"

${LangFileString} UnNotInRegistryLabel "Não foi possível encontrar o FuCad no Registro.$\r$\n\
					Os atalhos na área de trabalho e no Menu Iniciar não serão removidos."
${LangFileString} UnInstallRunning "É necessário fechar o FuCad primeiro!"
${LangFileString} UnNotAdminLabel "Para desinstalar o FuCad é necessário ter privilégios de administrador!"
${LangFileString} UnReallyRemoveLabel "Tem certeza que deseja remover completamente o FuCad e todos os seus componentes?"
${LangFileString} UnFuCadPreferencesTitle 'Preferências de usuário do FuCad'

#${LangFileString} SecUnProgDescription "Desinstala xxx."
${LangFileString} SecUnPreferencesDescription 'Exclui a configuração do FuCad$\r$\n\
						(pasta $\"$AppPre\username\$\r$\n\
						$AppSuff\$\r$\n\
						${APP_DIR_USERDATA}$\")$\r$\n\
						para você ou para todos os usuários (se você for um administrador)).'
${LangFileString} DialogUnPreferences 'You chose to delete the FuCads user configuration.$\r$\n\
						This will also delete all installed FuCad addons.$\r$\n\
						Do you agree with this?'
${LangFileString} SecUnProgramFilesDescription "Desinstalar o FuCad e todos os seus componentes."

${LangFileString} DirNotEmptyWarning "The selected folder '$INSTDIR' is not empty.$\r$\n\
                        The installer will remove all its content before installing. Continue?"
${LangFileString} RMInstDirFailed "Failed to remove '$INSTDIR'.$\r$\n\
                        Make sure you have sufficient permissions and that no files are in use."
