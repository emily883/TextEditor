#include <stdlib.h> // para atexits
#include <termios.h> // funciones y mas para controlar el terminal.
#include <unistd.h> // funciones de manejo descriptores de archivos


struct termios orig_termios; // Definir una estructura termios

void disableRawMode() { // Esta funcion restaura la configuracion original del terminal :)
  tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios); // restaura la config inicial que está dentro de orig_termios
}

void enableRawMode() {
  tcgetattr(STDIN_FILENO, &orig_termios); // obtiene la config inicial del terminal y lo guarda en orig_termios
  atexit(disableRawMode); // registra la funcion disableRawMode para que la llame al final del programa
  struct termios raw = orig_termios; // Crea una copia de la config original obvi
  raw.c_lflag &= ~(ECHO | ICANON); // desactiva el eco, o sea que lo que uno escriba no se verá :)
  tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw); // aplica la config modificada a la terminal
}

int main() {
  enableRawMode();
  char c;
  while (read(STDIN_FILENO, &c, 1) == 1 && c != 'q');
  return 0;
}
