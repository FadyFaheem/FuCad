/*
FuCad Installer Language File
Language: Spanish
*/

!insertmacro LANGFILE_EXT "Spanish"

${LangFileString} TEXT_INSTALL_CURRENTUSER "(Instalado para el actual usuario)"

${LangFileString} TEXT_WELCOME "Este programa instalará FuCad en su ordenador.$\r$\n\
				$\r$\n\
				$_CLICK"

#${LangFileString} TEXT_CONFIGURE_PYTHON "Compilando guiones Python..."

${LangFileString} TEXT_FINISH_DESKTOP "Crear acceso directo en el escritorio"
${LangFileString} TEXT_FINISH_WEBSITE "Visite fucad.org para últimas noticias, ayuda y consejos"

#${LangFileString} FileTypeTitle "Documento FuCad"

#${LangFileString} SecAllUsersTitle "Instalar para todos los usuarios"
${LangFileString} SecFileAssocTitle "Asociar ficheros"
${LangFileString} SecDesktopTitle "Icono de escritorio"

${LangFileString} SecCoreDescription "Los ficheros de FuCad."
#${LangFileString} SecAllUsersDescription "Instalar FuCad para todos los usuarios o sólo para el usuario actual."
${LangFileString} SecFileAssocDescription "Asociar la extensión .FCStd con FuCad."
${LangFileString} SecDesktopDescription "Crear un icono de FuCad en el escritorio."
#${LangFileString} SecDictionaries "Diccionarios"
#${LangFileString} SecDictionariesDescription "Diccionarios de revisión ortográfica que se pueden descargar e instalar."

#${LangFileString} PathName 'Ruta al fichero $\"xxx.exe$\"'
#${LangFileString} InvalidFolder 'Imposible encontrar $\"xxx.exe$\".'

#${LangFileString} DictionariesFailed 'La descarga del diccionario para el idioma $\"$R3$\" ha fallado.'

#${LangFileString} ConfigInfo "La siguiente configuración de FuCad va a tardar un poco."

#${LangFileString} RunConfigureFailed "Error al intentar ejecutar el programa de configuración"
${LangFileString} InstallRunning "El instalador ya está siendo ejecutado!"
${LangFileString} AlreadyInstalled "¡FuCad ${APP_SERIES_KEY2} ya está instalado!$\r$\n\
				Aún así, ¿quiere instalar FuCad sobre la versión existente?"
${LangFileString} NewerInstalled "Está tratando de instalar una versión de FuCad más antigua que la que tiene instalada.$\r$\n\
				  Si realmente lo desea, debe desinstalar antes la versión de FuCad instalada $OldVersionNumber."

#${LangFileString} FinishPageMessage "¡Enhorabuena! FuCad ha sido instalado con éxito.$\r$\n\
#					$\r$\n\
#					(El primer arranque de FuCad puede tardar algunos segundos.)"
${LangFileString} FinishPageRun "Ejecutar FuCad"

${LangFileString} UnNotInRegistryLabel "Imposible encontrar FuCad en el registro.$\r$\n\
					Los accesos rápidos del escritorio y del Menú de Inicio no serán eliminados."
${LangFileString} UnInstallRunning "Antes cierre FuCad!"
${LangFileString} UnNotAdminLabel "Necesita privilegios de administrador para desinstalar FuCad!"
${LangFileString} UnReallyRemoveLabel "¿Está seguro de que desea eliminar completamente FuCad y todos sus componentes?"
${LangFileString} UnFuCadPreferencesTitle 'Preferencias de usuario de FuCad'

#${LangFileString} SecUnProgDescription "Desinstala xxx."
${LangFileString} SecUnPreferencesDescription 'Elimina las carpetas de configuración de FuCad$\r$\n\
						$\"$AppPre\username\$\r$\n\
						$AppSuff\$\r$\n\
						${APP_DIR_USERDATA}$\")$\r$\n\
						de todos los usuarios.'
${LangFileString} DialogUnPreferences 'Eligió eliminar la configuración de usuario de FuCad.$\r$\n\
						Esto también eliminará todos los addons de FuCad instalados.$\r$\n\
						¿Está de acuerdo con esto?'
${LangFileString} SecUnProgramFilesDescription "Desinstala FuCad y todos sus componentes."

${LangFileString} DirNotEmptyWarning "La carpeta seleccionada '$INSTDIR' no está vacia.$\r$\n\
                        El instalador eliminará todo su contenido antes de instalar. ¿Continuar?"
${LangFileString} RMInstDirFailed "No se ha podido eliminar '$INSTDIR'.$\r$\n\
                        Asegúrese de tener suficientes permisos y que ningún archivo esté en use."
