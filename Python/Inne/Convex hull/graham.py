import math
from functools import cmp_to_key


class Vector:
    def __init__(self, x, y):
        self._x = float(x)
        self._y = float(y)

    @property
    def x(self):
        return self._x

    @x.setter
    def x(self, value):
        self._x = float(value)

    @property
    def y(self):
        return self._y

    @y.setter
    def y(self, value):
        self._y = float(value)

    def __str__(self):
        return f'Point({self._x}, {self._y})'


class Point:
    def __init__(self, x, y, index):
        self._x = float(x)
        self._y = float(y)
        self._index = int(index)

    @property
    def x(self):
        return self._x

    @property
    def y(self):
        return self._y

    @property
    def index(self):
        return self._index

    def __str__(self):
        return f'Point({self._x}, {self._y}, {self._index})'


def cross_product(v1: Vector, v2: Vector) -> float:
    """
    :param v1: pierwszy z wektorów, dla których będziemy liczyli iloczyn wektorowy
    :param v2: drugi z wektorów, dla których będziemy liczyli iloczyn wektorowy
    :return: iloczyn wektorowy dwóch wektorów
    """
    return -(v1.x * v2.y - v1.y * v2.x)


def distance(p0: Point, p1: Point) -> float:
    """
    :param p0: punkt p0
    :param p1: punkt p1
    :return: funkcja zwraca odległość między punktami p0 i p1
    """
    a = p1.x - p0.x
    b = p1.y - p0.y
    return math.sqrt(math.pow(a, 2) + math.pow(b, 2))



def function(name: str):

    return name

def comp_closure(p0: Point):
    """
    :param p0: punkt referencyjny
    :return: comp
    """
    def comp(p1: Point, p2: Point) -> int:
        """
        Komparator używany do sortowania listy punktów względem kąta z punktem referencyjnym.
        :param p1: Punkt do porównania
        :param p2: Punkt do porównania
        :return: Rezultat to albo p1 większy od p2 albo na odwrót
        """
        p0p1 = Vector(p1.x - p0.x, p1.y - p0.y)
        p0p2 = Vector(p2.x - p0.x, p2.y - p0.y)
        cp = cross_product(p0p1, p0p2)
        if cp > 0:
            return 1
        elif cp < 0:
            return -1
        else:
            collinear = distance(p0, p1) - distance(p0, p2)
            if collinear > 0:
                return 1
            else:
                return -1

    return comp


def find_smallest_yx(points_list):
    """
    :param points_list: lista obiektów - punktów
    :return: indeks punktu o najmniejszych koordynatach y
    """
    smallest = 0
    for i in range(1, len(points_list)):
        if points_list[i].y < points_list[smallest].y:
            smallest = i
        if points_list[i].y == points_list[smallest].y:
            if points_list[i].x < points_list[smallest].x:
                smallest = i
    return smallest


def graham_scan(points_list):
    """
    funkcja działa według kroków algorytmu Grahama tzn. :
        1. Znajdź punkt poczatkowy i potraktuj go jako nowy środek
            układu współrzędnych.
        2. Posortuj pozostałe punkty rosnąco według ich kąta względem osi X.
        3. Do powłoki wypukłej dodaj indeks punktu początkowego (nowy środek układu współrzędnych) oraz
            pierwszego punktu z posortowanego porządku.
        4. Przebiegaj pozostałe punkty w posortowanym porzadku (poczynając od drugiego):
            4.1 Dodaj indeks aktualnego punktu do powłoki wypukłej.
            4.2 Dopóki dwa wektory wyznaczone przez trzy ostatnie punkty leżące na aktualnej powłoce wypukłej tworzą „zakręt w prawo” (tzn. wektor ostatni „skręca w prawo” względem wektora
                przedostatniego):
            4.2.1 Usuń indeks przedostatniego punktu z aktualnej powłoki wypukłej.

    :param points_list: lista punktów 2d
    :return: lista punktów biorących udział w tworzeniu otoczki wypukłej
    """
    smallest = find_smallest_yx(points_list)

    point_list_no_ref = []
    for i in range(len(points_list)):
        if i != smallest:
            point_list_no_ref.append(points_list[i])
    reference = points_list[smallest]
    sorted_list_by_angle = sorted(point_list_no_ref, key=cmp_to_key(comp_closure(reference)))

    convex_hull = [reference, sorted_list_by_angle[0]]

    k = 1
    steps_history = []
    for j in range(1, len(sorted_list_by_angle)):
        convex_hull.append(sorted_list_by_angle[j])
        steps_history.append(convex_hull)
        k = k + 1
        v1 = Vector(convex_hull[k].x - convex_hull[k - 1].x, convex_hull[k].y - convex_hull[k - 1].y)
        v2 = Vector(convex_hull[k - 1].x - convex_hull[k - 2].x, convex_hull[k - 1].y - convex_hull[k - 2].y)
        while cross_product(v1, v2) < 0:
            convex_hull.pop(-2)
            steps_history.append(convex_hull)
            k = k - 1
            v1.x = convex_hull[k].x - convex_hull[k - 1].x
            v1.y = convex_hull[k].y - convex_hull[k - 1].y
            v2.x = convex_hull[k - 1].x - convex_hull[k - 2].x
            v2.y = convex_hull[k - 1].y - convex_hull[k - 2].y
    return convex_hull, steps_history

def read_points(file_name):
    """
    funkcja wczytuje z pliku listę punktów współrzędnych poszczególne punkty, a następnie zamienia je w pythonową listę obiektów klasy Point
    :param: file_name -  nazwa pliku .txt:
    :return: lista punktów 2d
    """
    points_list = []
    try:
        with open(file_name, 'r') as file:
            i = 0
            for line in file:
                attr = line.split()
                point = Point(float(attr[0]), float(attr[1]), i)
                points_list.append(point)
                i += 1
    except FileNotFoundError:
        print(f'Plik o nazwie {file_name} nie został znaleziony.')
    except Exception as e:
        print(f'Wystąpił błąd: {e}')

    return points_list

def coords_to_points(x,y):
    """

    :param x: lista koordynatów x
    :param y: lista koordynatów y
    :return: lista punktów
    """
    points_list = []
    for i in range(0,len(x)):
        point = Point(float(x[i]), float(y[i]), i)
        points_list.append(point)

    return points_list


points = read_points("points2.txt")
points1 = graham_scan(points)

