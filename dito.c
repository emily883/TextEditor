/*** includes ***/

#include <errno.h> // Te permite usar la variable errno para almacenar codigos de error y tmbn acceder a funciones para manejar y diagnosticar errores durante la ejecucion de un programa
#include <stdlib.h> // para atexits
#include <termios.h> // funciones y mas para controlar el terminal.
#include <unistd.h> // funciones de manejo descriptores de archivos
#include <ctype.h> // pa saber q caracter es, si es numero o es letra mayuscula o minuscula y tmbn convierte mayus a minus y cositas asi :v
#include <stdio.h> // para entrada y salida estandar uwu
#include <sys/ioctl.h>


/*** defines ***/

#define CTRL_KEY(k) ((k) & 0x1f)


/*** data ***/

struct editorConfig {
  int screenrows;
  int screencols;
  struct termios orig_termios; // Definir una estructura termios
};

struct editorConfig E;


/*** terminal ***/

void die(const char *s) {
  write(STDOUT_FILENO, "\x1b[2J", 4);
  write(STDOUT_FILENO, "\x1b[H", 3);

  perror(s); // nomas imprime el error :v
  exit(1);
}

void disableRawMode() { // Esta funcion restaura la configuracion original del terminal :)
   // tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios); restaura la config inicial que está dentro de orig_termios
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &E.orig_termios) == -1)
      die("tcsetattr");
}

void enableRawMode() {
  // tcgetattr(STDIN_FILENO, &orig_termios); // obtiene la config actual del terminal y lo guarda en orig_termios. tcgetattr() obtienes los atributos del terminal
  if (tcgetattr(STDIN_FILENO, &E.orig_termios) == -1) die("tcgetattr");
  atexit(disableRawMode); // registra la funcion disableRawMode para que la llame al final del programa sin importar que
  struct termios raw = E.orig_termios; // Crea una copia de la config original obvi, para asi poder modificar la copia sin dañar el original :)
  raw.c_iflag &= ~(BRKINT | ICRNL | IGNBRK | INPCK | ISTRIP | IXON);
// BRKINT: Controla si se genera una señal de interrupción cuando se recibe una señal de ruptura.
// ICRNL: Controla si los retornos de carro ('\r') se convierten automáticamente en saltos de línea ('\n').
// IGNBRK: Controla si se ignora el carácter de ruptura ('break').
// INPCK: Controla si se verifica la paridad de los datos de entrada.
// ISTRIP: Controla si se despoja el octavo bit del carácter de entrada.
// IXON: Controla si se habilita la funcionalidad de control de flujo de entrada, que interpreta ciertas combinaciones de teclas (como Ctrl+S y Ctrl+Q) como comandos de control de flujo.

  raw.c_oflag &= ~(OPOST); // el OPOST está relacionado con el procesamiento de salida, automaticamente el sistema operativo hace cambios como tabulaciones, saltos de linea, retorno de carro y asi. al desactivarlo te tirá la salida sin ninguna modificacion hecha por el sistema.
  raw.c_cflag |= (CS8);
  raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG); // desactiva el eco, o sea que lo que uno escriba no se verá :), el ICANON al desactivarlo la entrada se procesa al momento en q se escribe sin presionar enter. El ISIG manejo de señales, combinaciones de teclas las agarra como señales y no como un input del programa. el IEXTEN es basicamente lo mismo, toma algunas combinaciones como comandos, y al ponerlo aca se desactiva, entonces esas combinaciones se leen como inputs normalitos :)
  raw.c_cc[VMIN] = 0;
  raw.c_cc[VTIME] = 1;
  // tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw); // aplica la config modificada a la terminal
// el TCSAFLUSH asegura q el buffer de entrada y salida se limpien antes de aplicar los cambios en la configuracion del terminal :)
  if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1) die("tcsetattr"); // Si el aplicar la config no jala, tira errorsito mi amolll
}




/*** Terminal ***/

char editorReadKey() {
  int nread;
  char c;
  while ((nread = read(STDIN_FILENO, &c, 1)) != 1) {
    if (nread == -1 && errno != EAGAIN) die("read");
  }
  return c;
}


int getWindowSize(int *rows, int *cols) {
  struct winsize ws;

  if (1 || ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == -1 || ws.ws_col == 0) {
    if (write(STDOUT_FILENO, "\x1b[999C\x1b[999B", 12) != 12) return -1;
    editorReadKey();
    return -1;
  } else {
    *cols = ws.ws_col;
    *rows = ws.ws_row;
    return 0;
  }
}

/*** output ***/


void editorDrawRows() {
  int y;
  for (y = 0; y < E.screenrows; y++) {
    write(STDOUT_FILENO, "~\r\n", 3);
  }
}

void editorRefreshScreen() {
  write(STDOUT_FILENO, "\x1b[2J", 4); // escribir 4 bits, el \x1b es el caracter de escape es un byte, [2J el resto de bytes
  write(STDOUT_FILENO, "\x1b[H", 3); // pone el cursor de texto al inicio, el comando [ H acepta dos parametros

  editorDrawRows();

  write(STDOUT_FILENO, "\x1b[H", 3);

}

/*** input ***/

void editorProcessKeypress() {
  char c = editorReadKey();
  switch (c) {
    case CTRL_KEY('q'):
      write(STDOUT_FILENO, "\x1b[2J", 4);
      write(STDOUT_FILENO, "\x1b[H", 3);
      exit(0);
      break;
  }
}


/*** init ***/

void initEditor() {
  if (getWindowSize(&E.screenrows, &E.screencols) == -1) die("getWindowSize");
}

int main() {
  enableRawMode();
  initEditor();

  while (1){
    editorRefreshScreen();
    editorProcessKeypress();
  }

  return 0;
}
