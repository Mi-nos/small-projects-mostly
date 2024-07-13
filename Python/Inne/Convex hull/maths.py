import matplotlib.pyplot as plt
import numpy as np


#obliczanie pola wielokąta korzystając z wzoru Gaussa
def area(x, y):
    """

    :param x: lista koordynatów x
    :param y: lista koordynatów y
    :return: pole wielokąta
    """
    return 0.5 * np.abs(np.dot(x, np.roll(y, 1)) - np.dot(y, np.roll(x, 1)))


def plot_with_points(points_list, convex_hull_points):
    """

    :param points_list: lista punktów
    :param convex_hull_points: lista punktów biorących udział w otoczce wypukłej
    :return: wykres punktowy wraz z liniami łączącymi punkty z convex_hull_points
    """
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

    plt.scatter(x_coords_scatter, y_coords_scatter, marker='o', color='r', label='Punkty')
    plt.plot(x_coords_hull, y_coords_hull, linestyle='-', color='b', label='linie')
    plt.xlabel('X axis')
    plt.ylabel('Y axis')
    plt.title('Wykres punktowy')
    plt.legend()
    plt.show()


def scatter_with_points(points_list):
    '''

    :param points_list: lista punktow
    :return: wykres punktowy
    '''
    x_coords = []
    y_coords = []
    for i in range(0, len(points_list)):
        x_coords.append(points_list[i].x)
        y_coords.append(points_list[i].y)

    plt.scatter(x_coords,y_coords, label = 'Punkty')
    plt.xlabel('X axis')
    plt.ylabel('Y axis')
    plt.title('Wykres punktowy')
    plt.legend()
    plt.show()



