import sys
import maths
import graham
import time
import matplotlib.pyplot as plt
from PyQt5.QtWidgets import QApplication, QWidget, QPushButton, QVBoxLayout, QFileDialog
from matplotlib.backends.backend_qt5agg import FigureCanvasQTAgg as FigureCanvas


class Application(QWidget):
    def __init__(self):
        super().__init__()

        # Inicjalizacja Matplotlib
        self.figura, self.osie = plt.subplots()
        self.kanvas = FigureCanvas(self.figura)

        # Przycisk Rysuj
        self.przycisk_rysuj = QPushButton('Rysuj', self)
        self.przycisk_rysuj.clicked.connect(self.rysuj_wykres)
        self.przycisk_rysuj.setDisabled(True)

        # Przycisk Załaduj
        self.przycisk_zaladuj = QPushButton('Załaduj', self)
        self.przycisk_zaladuj.clicked.connect(self.zaladuj_punkty)

        # Przycisk Eksportuj
        self.przycisk_eksportuj = QPushButton('Eksportuj', self)
        self.przycisk_eksportuj.clicked.connect(self.eksportuj_do_png)

        # Przycisk Wyjdź
        self.przycisk_wyjdz = QPushButton('Wyjdź', self)
        self.przycisk_wyjdz.clicked.connect(self.close)

        # Dwie listy do przechowywania punktów x i y
        self._punkty_x = []
        self._punkty_y = []

        # Układ pionowy
        układ = QVBoxLayout(self)
        układ.addWidget(self.kanvas)
        układ.addWidget(self.przycisk_rysuj)
        układ.addWidget(self.przycisk_zaladuj)
        układ.addWidget(self.przycisk_eksportuj)
        układ.addWidget(self.przycisk_wyjdz)
        # Ustawienie układu dla okna
        self.setLayout(układ)

    @property
    def punkty_x(self):
        return self._punkty_x

    @punkty_x.setter
    def punkty_x(self, nowe_punkty_x):
        self._punkty_x = nowe_punkty_x

    @property
    def punkty_y(self):
        return self._punkty_y

    @punkty_y.setter
    def punkty_y(self, nowe_punkty_y):
        self._punkty_y = nowe_punkty_y

    def rysuj_wykres(self):
        '''
        funkcja rysuje drogę otoczki wypukłej na wykresie punktowym i wyswietla pole otoczki
        :return:
        '''
        # Rysowanie wykresu
        self.osie.clear()
        points = graham.coords_to_points(self.punkty_x, self.punkty_y)
        points_hull, steps = graham.graham_scan(points)
        points_hull.append(points_hull[0])
        plot_graham(self.osie, points, points_hull, steps)
        self.kanvas.draw()

    def rysuj_droge(self):
        self.osie.clear()
        points = graham.coords_to_points(self.punkty_x, self.punkty_y)
        points_hull, steps = graham.graham_scan(points)
        points_hull.append(points_hull[0])
        x_coords, y_coords = points_to_lists(points)
        x_steps, y_steps = point_lists_to_lists(steps)
        for i in range(0, len(steps)):
            plot_line(self.osie, x_coords, y_coords, x_steps[i], y_steps[i])
            self.kanvas.draw()
            time.sleep(0.05)
            self.osie.clear()
    def zaladuj_punkty(self):
        # Wybierz plik tekstowy
        nazwa_pliku, _ = QFileDialog.getOpenFileName(self, "Załaduj punkty z pliku", "", "Text files (*.txt);;All Files (*)")

        if nazwa_pliku:
            # Wczytaj punkty z pliku tekstowego
            self.punkty_x, self.punkty_y = self.wczytaj_punkty_z_pliku(nazwa_pliku)

            self.osie.clear()
            scatter_with_points(self.osie, self.punkty_x, self.punkty_y)
            self.kanvas.draw()

            # Włącz przycisk "Rysuj" po załadowaniu punktów
            self.przycisk_rysuj.setDisabled(False)

    def wczytaj_punkty_z_pliku(self, nazwa_pliku):
        x, y = [], []
        try:
            with open(nazwa_pliku, 'r') as plik:
                for linia in plik:
                    punkt = linia.strip().split()
                    if len(punkt) == 2:
                        x.append(float(punkt[0]))
                        y.append(float(punkt[1]))
        except Exception as e:
            print(f'Błąd podczas wczytywania pliku: {e}')

        return x, y

    def eksportuj_do_png(self):
        # Wybierz miejsce do zapisu pliku
        nazwa_pliku, _ = QFileDialog.getSaveFileName(self, "Zapisz obraz wykresu", "", "PNG (*.png)")

        if nazwa_pliku:
            # Zapisz obraz do pliku PNG
            self.figura.savefig(nazwa_pliku, format='png')
            print(f'Obraz został zapisany do: {nazwa_pliku}')


def plot_line(axes, x_coords, y_coords, line_x, line_y):
    axes.scatter(x_coords, y_coords, label='Punkty')
    axes.plot(line_x, line_y, linestyle='-', color='b', label='linie')
    axes.set_xlabel('Oś X')
    axes.set_ylabel('Oś Y')
    axes.set_title('Wykres drogi algorytmu Grahama')
    axes.legend()

def scatter_with_points(axes, x_coords, y_coords):

    axes.scatter(x_coords, y_coords, label='Punkty')
    axes.set_xlabel('Oś X')
    axes.set_ylabel('Oś Y')
    axes.set_title('Wykres wszystkich punktów')
    axes.legend()

def points_to_lists(points):
    x_coords = []
    y_coords = []
    for point in points:
        x_coords.append(points.x)
        y_coords.append(points.y)
    return x_coords, y_coords
def point_lists_to_lists(steps):
    x_steps = []
    y_steps = []
    for step in steps:
        temp_x = []
        temp_y = []
        for point in step:
            temp_x.append(point.x)
            temp_y.append(point.y)
        x_steps.append(temp_x)
        y_steps.append(temp_y)
    return x_steps, y_steps


def plot_graham(axes, points_list, convex_hull_points, steps):
    '''
    funkcja rysuje wykres punktowy, łączy punkty otoczki wypukłej i oblicza pole otoczki
    '''
    x_coords_scatter = []
    y_coords_scatter = []
    for i in range(0, len(points_list)):
        x_coords_scatter.append(points_list[i].x)
        y_coords_scatter.append(points_list[i].y)

    x_coords_hull = []
    y_coords_hull = []
    for i in range(0, len(convex_hull_points)):
        x_coords_hull.append(convex_hull_points[i].x)
        y_coords_hull.append(convex_hull_points[i].y)

    x_steps = []
    y_steps = []
    for step in steps:
        temp_x = []
        temp_y = []
        for point in step:
            temp_x.append(point.x)
            temp_y.append(point.y)
        x_steps.append(temp_x)
        y_steps.append(temp_y)

 #   axes.scatter(x_coords_scatter, y_coords_scatter, label='Punkty')
    for i in range(0, len(x_steps)):
        axes.scatter(x_coords_scatter, y_coords_scatter, label='Punkty')
        axes.plot(x_steps[i], y_steps[i], linestyle='dashdot', color='y', label='droga tymczasowa')
        time.sleep(0.1)
        axes.clear()

    axes.scatter(x_coords_scatter, y_coords_scatter, label='Punkty')
    axes.plot(x_coords_hull, y_coords_hull, linestyle='-', color='r', label='droga algorytmu Grahama')
    axes.fill(x_coords_hull, y_coords_hull, color='purple', alpha=0.8)
    area = maths.area(x_coords_hull, y_coords_hull)
    srodek_x = sum(x_coords_hull) / len(x_coords_hull)
    srodek_y = sum(y_coords_hull) / len(y_coords_hull)
    axes.text(srodek_x, srodek_y, f'Pole: {area:.2f}', ha='center', va='center', fontsize=24, color='white')
    axes.set_xlabel('Oś X')
    axes.set_ylabel('Oś Y')
    axes.set_title('Wykres wszystkich punktów wraz z otoczką wypukłą')
    axes.legend()


if __name__ == '__main__':
    app = QApplication(sys.argv)
    okno = Application()
    okno.show()
    sys.exit(app.exec_())