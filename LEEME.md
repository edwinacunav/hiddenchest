# HiddenChest

HiddenChest es un proyecto basado en mkxp desarrollado por Ancurio. Les permite jugar videojuegos basados en RGSS con una resolución de al menos 800 * 608 o superior por defecto, pero pueden cambiarlo antes de compilarlo de ser necesario. En mi caso yo definí la máxima en 1920 * 1080, aunque solo lo he llegado a probar a 1680 * 1050 a lo sumo. Los resultados fueron bastantes decentes.

Después de que hayan finalizado la compilación del ejecutable binario, deberían de abrir el archivo de texto MapCustomResFixes (para juegos de RMXP o RMVX) con un editor como Notepad++ o cambiar su extensión a txt si buscan usar la aplicación de Windows llamada Notepad. En distribuciones de Linux pueden utilizar su editor de texto favorito, para un GUI de KDE Kate o KWrite estarían bien. Una vez que lo hayan abierto, copien su contenido y péguenlo en el editor de scripts debajo de Scene_Debug y sobre Main.

¡El método `Sprite#mirror_y` alias `Sprite#flip_y` ha sido añadido!

Excepto por muchas herramientas de scripting, las opciones de apertura de ventanas (ya dentro del juego), la resolución incrementada, el reflejo del eje Y en imágenes, el teclado completo y varios arreglos de errores, este proyecto retiene mucho del mkxp original.

La documentación de mkxp aún es válida en buena medida por lo que podrían revisarla.

## Sobre mkxp

[mkxp](https://github.com/Ancurio/mkxp) es un proyecto que busca proveerles una implementación de código abierto (open source) de la interfaz de Ruby Game Scripting System (RGSS) usada en el popular software de creación de juegos "RPG Maker XP", "RPG Maker VX" y "RPG Maker VX Ace" (marcas registradas de Enterbrain, Inc. o Kadokawa), con enfoque en Linux principalmente. La meta es correr los juegos creados con aquellas aplicaciones de forma nativa esperando no tener que cambiar ningún archivo.

Está bajo la licencia GNU General Public License v2+.

## mkxp: Binarios Precompilados con 640px de Ancho
[**Linux (32bit/64bit)**](http://ancurio.bplaced.net/mkxp/generic/)
[**OSX**](https://app.box.com/mkxpmacbuilds) por Ali
[**Windows (mingw-w64 32bit)**](http://ancurio.bplaced.net/mkxp/mingw32/)

## HiddenChest: Binarios Precompilados con +800px de Ancho
[**Ubuntu or Fedora 64bit**](https://app.box.com/s/8956vvm2spfx5sdmeo2jrrtanawsoj1j)
[**Windows 64bit**](https://app.box.com/s/65lwn59sk7lm25hqrlws2lq5r5ye4yod)

Si planeas depender de juegos con un ancho de 640 píxeles wide games, deberían de ir tras mkxp.

## Bindings
Bindings les proveen de código para unir un lenguaje interpretado como Ruby con un ambiente capaz de correr scripts de juegos. Actualmente hay tres bindings:

### MRI
Página Web: https://www.ruby-lang.org/en/ 

Matz's Ruby Interpreter, también llamado CRuby por su API escrita en C, es la versión de Ruby más distribuida por la red. Si están interesados en correr juegos creados en RPG Maker XP u otros, deben de escoger esta opción. MRI 1.8 fue lo que se utilizó en RPG Maker XP; sin embargo, este binding se escribió contra el 2.6 (la versión estable en la actualidad). Para juegos que solo usen los scripts por defecto provistos por Enterbrain, este binding funciona bastante bien.

Noten que hay diferencias en el lenguaje y la síntaxis entre 1.8 y 2.6, así que scripts creados por algún usuario tal vez no funcionen correctamente. La version 2.6 no los perdonará si usan la vieja síntaxis como en los condicionales case en los que se acostumbró a dejar : dos puntos después de una condición when condition, por ejemplo: when 2 : call_battle. Esos dos puntos deben ser reemplazados por la palabra reservada `then` para mantener la compatibilidad con posteriores versiones de Ruby.

Para leer una lista de diferencias, visiten:
http://stackoverflow.com/questions/21574/what-is-the-difference-between-ruby-1-8-and-ruby-1-9

Este binding soporta RGSS1, RGSS2 y RGSS3.

### mruby (Lightweight Ruby)
Página Web: https://github.com/mruby/mruby

mruby es un nuevo proyecto de Matz y otros para crear una implementación de Ruby más ligera, que se adhiera a las especificaciones y embebible. Pueden verlo como la versión Ruby de Lua.

Debido a las serias diferencias entre mruby y MRI como a la carencia de módulos, correr juegos de RPG Maker con este binding fallará muy probablemente. Se les ha provisto como código experimental. Pueden, por ejemplo, escribir sus propios scripts de Ruby scripts y ejecutarlos.

Algunas extensiones a las clases/módulos estándares les han sido provistas, tomando el archivo de ayuda CHM de RPG Maker XP como una casi "referencia". Esto incluye Marshal, File, FileTest y Time.

Este binding solo soporta RGSS1.

**Importante:** Si deciden utilizar [oniguruma regexp gem de mattn](https://github.com/mattn/mruby-onig-regexp), no olviden agregar `-lonig` a las flags del linker para previnir horribles reescrituras de símbolos en libc.

Ha sido reportado que el Array de mruby no soporta variables de instancia y el método puts despliega sus argumentos como un arreglo sin ningún salto de línea.

### null
Este binding solo existe para propósito de pruebas y no hace nada (el engine se cierra inmediatamente). Puede ser utilizado para cosas como ejecutar un bucle de juego de RGSS mínimo de forma directa en C++.

## Dependencias / Compilación

* Boost.Unordered (solo headers)
* Boost.Program_options
* libsigc++ 2.0
* PhysFS (más reciente hg)
* OpenAL
* SDL2*
* SDL2_image
* SDL2_ttf
* [SDL_sound fork de Ancurio](https://github.com/Ancurio/SDL_sound)
* vorbisfile
* pixman
* zlib (solo bindings de Ruby)
* OpenGL header (alternativamente GLES2 con `DEFINES+=GLES2_HEADER`)
* libiconv (en Windows, opcional con INI_ENCODING)
* libguess (opcional con INI_ENCODING)

(* Para que el menú F1 funcione correctamente en Linux/X11, necesitan el más reciente hg + [este parche](https://bugzilla.libsdl.org/show_bug.cgi?id=2745))

HiddenChest y mkxp pueden emplear el sistema de compilación qmake de Qt, así que necesitarán instalarlo de antemano. Alternativamente pueden compilarlo con cmake con éxito y hasta compilarlo para otros sistemas operativos.


qmake usará pkg-config para localizar las respectivas rutas de include y lib. Si han instalado algunas dependencias con prefijos no estándares, asegúrense de ajustar su variable `PKG_CONFIG_PATH` a sus necesidades.

Sobre Boost sírvanse leer la sección de Boost en esta guía.

### CMake

Pueden compilar con CMake al ejecutar mi script de bash compile.sh en su terminal al digitar lo siguiente en el directorio raíz de su proyecto:

./compile.sh

Podrían necesitar el uso del comando chmod +x compile.sh para convertirlo en un script ejecutable antes de correrlo.

Creará un directorio build donde ejecutará CMake y luego correrá make para compilar el binario de HiddenChest. Si tienen éxito, trasladará el ejecutable binario a la raíz del directorio por ustedes.

Tomen en cuenta que necesitarán el fork de Ancurio de SDL_sound, ya instalado en su sistema. El administrador de paquetería de su Sistema Operativo, ya sea pacman o apt-get o yum, podría ofrecerles la instalación de una versión por defecto de SDL_sound. ¡NO LO INSTALEN! Tanto HiddenChest como mkxp ¡fallarán de seguro si no siguen mi consejo!

Busquen `set(MRIVERSION` en el archivo CMakeLists.txt para fijar una versión diferente de Ruby. Por defecto fijé la 2.6.

### Boost

La excepción es boost, lo que es raro porque aún no ha logrado que incorporen soporte de pkg-config (¿En serio?). Si han instalado boost en un prefijo no estándar, necesitarán pasar su dirección de include path via BOOST_I y su biblioteca (library) via BOOST_L, ya sea como argumentos directos a qmake (qmake BOOST_I="/usr/include" ...) o via variables del entorno. Pueden especificar un sufijo de biblioteca como "-mt" via BOOST_LIB_SUFFIX de ser necesario.

### Soporte de MIDI

El soporte de Midi está habilitado por defecto y requiere de fluidsynth a la hora de ejecución (no para compilarlo). Si HiddenChest no puede hallarlo al ejecutarse, la reproducción de midi se deshabilita. Buscará `libfluidsynth.so.1` en Linux, `libfluidsynth.dylib.1` en OSX y `fluidsynth.dll` en Windows, así que asegúrense de tener uno en la configuración de sus directorios. Si necesitan que fluidsynth sea enlazado estrechamente al engine al compilarlo, incluyan `CONFIG+=SHARED_FLUID`. De compilar fluidsynth por ustedes mismos, pueden deshabilitar casi todas las opciones (controladores de audio, etc.) ya que no se usarán. Noten que upstream fluidsynth ofrece soporte de compartir datos de un soundfont entre sintetizadores (HiddenChest usa múltiples sintetizadores), por lo que si su consumo de memoria llega a ser muy alto, podrían querer compilar fluidsynth desde su git master.

Por defecto HiddenChest cambia a la carpeta donde su binario se halla y entonces allí comienza a leer la configuración y resuelve las rutas relativas. En el caso de que eso sea indeseable como cuando el binario debe de instalarse globalmente, en una ubicación de solo lectura, puede ser deshabilitada al agregar `DEFINES+=WORKDIR_CURRENT` a los argumentos de qmake.

Para detectar automáticamente el encoding del título del juego en `Game.ini` y convertirlo automáticamente a UTF-8, compilen con `CONFIG+=INI_ENCODING`. Requiere de una implementación de iconv y libguess. Si el encoding es detectado incorrectamente, pueden fijar la seña "titleLanguage" en el archivo hiddenchest.conf.

**Binding de MRI**: pkg-config buscará `ruby-2.6.pc`, pero puede ser reemplazado por otra versión con `MRIVERSION=2.2` ('2.2' sería el ejemplo). Este es el binding por defecto, así que no se necesitan argumentos para qmake (`BINDING=MRI` para ser más claros).

**Binding de MRuby**: coloque la carpeta "mruby" dentro del proyecto y compílenlo primero. Agreguen `BINDING=MRUBY` a los argumentos de qmake.

**Binding Null**: Agreguen `BINDING=NULL` a los argumentos de qmake.

### Soporte de Formatos de Imágenes y Audio

Dependen de las bibliotecas auxiliares de SDL2. Para una mejor adherencia a RGSS, compilen SDL2_image con soporte de png/jpg y SDL_sound con soporte de oggvorbis/wav/mp3.

Para ejecutar HiddenChest, deben de tener una tarjeta o placa gráfica capaz de correr al menos **OpenGL (ES) 2.0** con un controlador actualizado ya instalado.

## Kit de Dependencias

Para simplificar el hackeo, Ancurio ha ensamblado un paquete que contiene todas las dependencias para compilar mkxp en un sistema Ubuntu 12.04 de 64 bit fresco, pero yo compilo HiddenChest en una instalación de Ubuntu 18.04 LTS de 64 bit. La compatibilidad con otras distribuciones de Linux no ha sido comprobada. Pueden descargarlo desde [here](https://www.dropbox.com/s/mtp44ur367m2zts/mkxp-depkit.tar.xz). Lean el archivo "README.txt" para más instrucciones.

## Configuración

HiddenChest lee los datos de configuración del archivo "hiddenchest.conf". El formato es de tipo ini. *No* usen comillas alrededor de las rutas de archivos (los espacios no lo interrumpen). Las líneas que comienzan con '#' son simples comentarios. Vean 'hiddenchest.conf.sample' para una lista de entradas aceptables.

All option entries can alternatively be specified as command line options. Any options that are not arrays (eg. RTP paths) specified as command line options will override entries in hiddenchest.conf. Note that you will have to wrap values containing spaces in quotes (unlike in hiddenchest.conf).

La síntaxis es: `--<option>=<value>`

Ejemplo: `./hiddenchest --gameFolder="mi_juego" --vsync=true --fixedFramerate=60`

## Música Midi

HiddenChest no viene con una soundfont por defecto, así que deben de suplirla ustedes mismos (fijen su dirección en el config). La reproducción ha sido probada y debería reproducir bastante bien todos los recursos del RTP.

Pueden utilizar esta soundfont de dominio público: [GMGSx.sf2](https://www.dropbox.com/s/qxdvoxxcexsvn43/GMGSx.sf2?dl=0)

## Fonts

En la versión de RMXP del RGSS, las letras o fuentes o tipos se cargaban directamente desde las rutas específicas del sistema, lo que implica que debían instalarse para estar disponibles en sus juegos. Como eso es un dolor gigantesco que depende de cada plataforma, se decidió que se implementaría la conducta que gracias a Enterbrain se agregoó a RPG Maker VX Ace: cargar las fuentes se realiza al buscar la carpeta llamada "Fonts", lo cual obedece a la conducta por defecto de búsqueda de rutas como que pueda estar ubicada en la carpeta del juego o de un RTP).

Si una fuente solicitada no es hallada, no se genera ningún error. En su lugar una fuente preincluida es utilizada, actualmente es "FreeSans".

## Lo que todavía no funciona

* Reproducción de vídeos
* Archivos de audio WMA
* La clase de Ruby llamada Win32API (por razones obvias)
* Creación de Bitmaps con tamaños superiores al límite de textura de OpenGL (Probablemente sea de cerca de 8192 en algunas tarjetas o placas.)

\* Existe una excepción llamada *mega surface* (mega superficie). Cuando un Bitmap mayor al límite es creado desde un archivo, no se guarda en el VRAM sino en el RAM. Su único propósito es el de ser usado como bitmap de tileset. Cualquier otra operación realizada en ella (además de usar blitting en un Bitmap común) generará un error.

## Nuevas Características alias Extensiones de RGSS No Estándares

Para aliviar la posible incorporación de scripts demasiado dependientes de la clase Win32API, se han añadido ciertas funcionalidades que no hallarán en las especificaciones de ningún RGSS. Actualmente esto comprende lo siguiente:

* Un módulo especial ha sido incluido, es `HIDDENCHEST`. Sus constantes consisten de `AUTHOR`, `VERSION`, `RELEASE_DATE`, y `DESCRIPTION`.
* La constante `OS::NAME` permitirá a scripts de Ruby saber si su Sistema Operativo es una distribución de Linux o Windows.
* Expandidos tilesets de RGSS1 para llenar la pantalla con una mayor resolución de pantalla y adapta las configuraciones de Window_Message a dicho tamaño.
* Cuando se ejecutaba un exe de Windows que tenía una resolución idéntica a la del sistema operativo, el juego automáticamente corría en modo de pantalla completa. Ese no era el caso de los binarios de Linux. HiddenChest ahora soporta este comportamiento en todas las plataformas.
* Implementación de window openness (apertura y cierre de ventanas) en juegos de RMXP sin afectar las que existieran en VX y VX Ace. Funciona de una forma algo diferente a la de VX Ace como el uso de valores en un rango de 0 a 100 cuando en VX Ace se requería de un máximo de 255.
* Hay 4 modos disponibles para asignar a la opción `self.open_mode` como lo son:
    - nil o false o true - nada pasa jamás
    - :top - la ventana se estira desde los extremos superior e inferior
    - :center - la ventana se abre como un rollo en direcciones este y oeste
    - :bottom - la ventana parece crecer como una planta
        - Tengan en cuenta que la orden `Window#open` será ignorada si un valor booleano es fijado como valor actual de `Window#open_mode`.
        - Pueden fijar el valor de `Window#open_mode=` antes o después de que la ventana haya llamado a su superclase via `super` al inicializarse o crearse dicha ventana.
        - Solo el último valor asignado a `Window#open_mode=` será tomado en cuenta por el engine.
* Tanto `Window#set_xy(newx, newy)` como `Sprite#set_xy(newx, newy)` les permiten asignar ambas coordenadas cartesianas al mismo tiempo.
* Asignen un Viewport a cualquier ventana de RGSS1 usando `Window#viewport = un_viewport`
* Se soportan muchas más teclas como PrintScreen, Return o Enter o LeftShift o RightAlt o NumPadDivide * o KeyH o KeyM o desde N1 hasta N0 para los números ubicados sobre las letras comunes como N y muchas más.
* Clases `Bitmap`, `Sprite` y `Window` ahora soportan clics del ratón! Bueno lo hacen indirectamente... Necesitarán usar el arreglo `@area` con cada una de las dimensiones x, y, ancho y altura. Usualmente lo harían en `Window_Selectable` y sus subclases en el método refresh. Los siguientes llamados a script pueden ser utilizados en scripts de escenas:
    - `Bitmap#alpha_pixel?(x, y)` - Pixel con alfa de 0. ¡No es solo para clics!
    - `Sprite#mouse_above?` alias `Sprite#mouse_inside?` - Ratón sobre Sprite
    - `Sprite#mouse_above_color?` - Ignora píxeles con valor alfa de 0.
    - `Window#mouse_above?(indice_de_opcion)` alias `Window#mouse_inside?(indice_de_opcion)` - Ratón sobre Ventana
* Clase `Bitmap` soporta el método 'invert!'. ¡Invertirá los colores actuales del Bitmap en instantes!
* ¡El engine se engalana con la clase totalmente nueva de `MsgBoxSprite`!
    - `bitmap` maneja sus contenidos básicos, los usuales.
    - `bar_bitmap` está hecho de una barra de título como en Windows y KDE y GTK.
    - `close_icon` no es más que el ícono donde hacen clic para cerrar la ventana!
    - `contents` representa al contenedor de todos los mensajes de textos impresos en ella!
* Módulo `Graphics` tiene tres propiedades adicionales: `fullscreen` representa al modo actual de pantalla completa (`true` = pantalla completa, `false` = en ventana), `show_cursor` esconde el cursor del Sistema Operativo de estar dentro de la ventana de juego si se fija en `false` y `block_fullscreen` (`true` o `false`) impedirá o permitirá que el jugador entre en modo de pantalla completa incluso si cambian la configuración en el archivo de configuración hiddenchest.conf.
* Módulo `Graphics` también permite hacer capturas de pantalla al llamar al método `save_screenshot` (guardado en la carpeta Screenshots) u obtener su ancho y su altura en un arreglo con `dimensions`.
* Módulo `Settings` permite personalizar las configuraciones de su juego via llamados a script.
    - `image_format` y `image_format=` permiten verificar o asignar el formato preferido de imágenes para sus capturas. Opciones disponibles:
         - :jpg o 0 para formato JPG - opción por defecto
         - :png o 1 para formato PNG
    - `snapshot_dir` y `snapshot_filename` para definir la carpeta y el nombre base de los archivos de capturas de pantalla.
    - `save_dir` y `save_filename` para definir la carpeta y el nombre base de los archivos de partidas guardadas.
    - `auto_create_dirs` para que se creen las carpetas si cambiaron la ruta de alguna de las carpetas.
* Módulo `Game` incluye las constants `TITLE` y `VERSION`. Estas son extraídas del archivo Game.ini
* Módulo `Backdrop`, disponibles en todas las versiones de RGSS, permite crear una captura temporal del mapa previo para usarlo como fondo de una escena distinta. Usen cualquiera de los siguientes llamados para crear el bitmap que necesitarán en su escena personalizada. Después pueden asignarle su bitmap a una variable de @instancia de su elección al llamar a su método `bitmap` o `bitmap_dup` (duplicado). Luego de congelar la escena con `Graphics.freeze`, llamen a `clear_bitmap` para desecharlo apropiadamente o desechen el bitmap de su sprite directamente en caso de que deseen guardarlo para más adelante y hayan utilizado el método `bitmap_dup` anteriormente.
    - `keep_bitmap` - Su mapa sin efectos especiales
    - `gray_bitmap` - Versión en Gris de su mapa
    - `sepia_bitmap` - Versión en Sepia de su mapa
    - `blur_bitmap` - Versión Borrosa de su mapa
    - `color_bitmap` - Escojan un color para teñir su mapa. Opciones:  :red, :green, :blue, :yellow, :sepia and :gray (rojo, verde, azul, amarillo, sepia y gris)
* Sprites ahora soportan versiones en gris y tono sepia de sus bitmaps! Usen un booleano (true o false) para activar o desactivarlos efectos de color.
    - `gray_out = booleano` - gris
    - `turn_sepia = booleano` - sepia
    - `invert_colors = booleano` - inversión de colores
    - `grayed_out?` - Para preguntar si ya está en tonos de gris
    - `sepia?` - Para preguntar si ya tiene un tono en sepia.
    - `colors_inverted?` - Por si neceistan verificar si los colores fueron invertidos
* Usen el método `module_accessor` para crear métodos de módulo, getters y setters ¡todo en uno! Ejemplo: `module_attr_accessor :meow` creará los métodos `self.meow` y `self.meow=(valor)` en un solo paso. Sus setter y getter son `module_writer` y `module_reader` respectivamente.
* Módulo `Scripts` permite que guarden en una cadena de texto o String de Ruby o en un símbolo como un ID de script via `Scripts << :nombre_script`. Una vez que sean guardados allí, podrán llamar métodos como `Scripts.all` o `Scripts.include?(:nombre_script)` para acceder a los IDs de arreglos (arrays) o confirmar si ya los han incluido.
* `RPG::Weather.sprite_max = numero`. donde numero es un número entero positivo, permite que definan el máximo de sprites de clima como los de efectos de lluvia o tormenta o nieve. Actualmente está fijado en 400 sprites, pero puede ser elevado o disminuido cuando sea necesario.
* Clase `FileInt` permite que pregunten si un archivo existe `exist?` incluso si está comprimido en el RGSSAD.
* Módulo `Audio` incluye más métodos como `bgm_volume`, `bgs_volume`, `se_volume` y `me_volume`.

### Módulo Input

* La familia de funciones `Input.press?` acepta varias constantes adicionales:
    - `MOUSELEFT` o `MouseLeft`
    - `MOUSEMIDDLE` o `MouseMiddle`
    - `MOUSERIGHT` o `MouseRight`
* Funciones adicionales:
    - `mouse_x` y `mouse_y` para preguntar por la posición del puntero del ratón relativo a la pantalla de juego
    - `dir4?` y `dir8?` para evitarles el uso de 4 u 8 condicionales en fila.
    - `press_all?` y `trigger_any?` para evitarles el uso de varias condicionales en fila.
         - Pueden pasar un argumento tras otro o pasar un Array alias arreglo.
    - `left_click?`, `middle_click?` y `right_click?`
* Se pueden usar :símbolos en todas las versiones de RGSS.

## Clase Font

* Les permite usar varios métodos nuevos que manejan valores booleanos (true o false) y tamaños (Integer o entero)
    - `underline` - booleano - subrayado
    - `strikethrough` o `strikethru` - booleano - tachado
    - `no_squeeze` - booleano, valor por defecto: false
    - `outline_size` - entero entre 1 y 8 - grosor del contorno
    - `shadow_size` - entero entre 1 y 3 - grosor del contorno
    - `shadow_color` - Color en formato RGBA (Rojo Verde Azul Alfa)

## Lista de Arreglos de Errores en HiddenChest

* Depurado error por el que el windowskin desaparecía para todas las ventanas existentes en la misma escena o menú si solo una de ellas era desechada con dispose.
* Arreglado error por el que se notaba que las teclas ALT, CTRL y SHIFT se revisaban continuamente de forma indeseable y sin motivo.
* Arreglado el error del Reinicio del F12 de forma ¡permanente!
* Arreglada la clase Sprite al prevenir que fijara un bitmap no existente o nil como si fuera el bitmap original cuando nunca había sido modificado para ver efectos de tonos de gris o sepia.
