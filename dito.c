/*** includes ***/

#include <errno.h> // Te permite usar la variable errno para almacenar codigos de error y tmbn acceder a funciones para manejar y diagnosticar errores durante la ejecucion de un programa
#include <stdlib.h> // para atexits
#include <termios.h> // funciones y mas para controlar el terminal.
#include <unistd.h> // funciones de manejo descriptores de archivos
#include <ctype.h> // pa saber q caracter es, si es numero o es letra mayuscula o minuscula y tmbn convierte mayus a minus y cositas asi :v
#include <stdio.h> // para entrada y salida estandar uwu


/*** data ***/

struct termios orig_termios; // Definir una estructura termios


/*** terminal ***/

void die(const char *s) {
  perror(s); // nomas imprime el error :v
  exit(1);
}

void disableRawMode() { // Esta funcion restaura la configuracion original del terminal :)
   // tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios); restaura la config inicial que está dentro de orig_termios
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios) == -1)
      die("tcsetattr");
}

void enableRawMode() {
  // tcgetattr(STDIN_FILENO, &orig_termios); // obtiene la config actual del terminal y lo guarda en orig_termios. tcgetattr() obtienes los atributos del terminal
  if (tcgetattr(STDIN_FILENO, &orig_termios) == -1) die("tcgetattr");
  atexit(disableRawMode); // registra la funcion disableRawMode para que la llame al final del programa sin importar que
  struct termios raw = orig_termios; // Crea una copia de la config original obvi, para asi poder modificar la copia sin dañar el original :)
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


/*** init ***/

int main() {
  enableRawMode();
  while (1){
    char c = '\0';
    if(read(STDIN_FILENO, &c, 1) == -1 && errno != EAGAIN) die("read");
    if(iscntrl(c)){ // el iscntrl verifica si la entrada es un caracter de control, osea un caracter sin representacion visal :v, si lo es pues retorna true
      printf("%d\r\n", c);
    }else{
      printf("%d ('%c')\r\n", c, c); // %d da referencia al primer c solito que imprime el valor numerico del caracter, y el ('%c') hace referencia al segundo c que imprimirá el c deporsé :)
    }
    if(c == 'q') break;
  }
  return 0;
}
