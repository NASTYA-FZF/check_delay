from matplotlib import cm
import matplotlib.pyplot as plt
import numpy as np
import os
import tkinter as tk
import sys


def modal_window(title, text):
    # Создание основного окна
    root = tk.Tk()
    root.title(title)
    root.geometry("300x100")

    # Метка с текстом
    label = tk.Label(root, text=text, font=("Arial", 14))
    label.pack(pady=20)

    root.mainloop()


def prepare_data(data):
    pointX = []
    pointY = []
    pointZ = []
    labels = []
    vertical_lines = []
    mode = "2d"
    for d in data:
        name, value = d.split(":")
        if name == "title":
            plt.title(value)
            continue
        elif name == "xlabel":
            plt.xlabel(value)
            continue
        elif name == "ylabel":
            plt.ylabel(value)
            continue
        elif name == "pointX":
            pointX.append([float(i) for i in value.split()])
            continue
        elif name == "pointY":
            pointY.append([float(i) for i in value.split()])
            continue
        elif name == "pointZ":
            pointZ.append([float(i) for i in value.split()])
            continue
        elif name == "label":
            labels.append(value)
            continue
        elif name == "axvline":
            vertical_lines = value.split()
            continue
        elif name == "mode":
            mode = value
            continue
    if mode == "2d":
        for i in range(len(pointX)):
            if len(labels) > 0:
                plt.plot(pointX[i], pointY[i], label=labels[i])
            else:
                plt.plot(pointX[i], pointY[i])
        if len(labels) > 0:
            plt.legend()
        for x in vertical_lines:
            if x.isdigit() or len(x.split(".")) == 2:
                plt.axvline(x=float(x), color="r", linestyle="--")
        plt.grid(True)
    else:

        stepX = int(len(pointX[0]) / 15)
        stepY = int(len(pointY[0]) / 15)
        x_ticks = [i for i in range(0, len(pointX[0]), stepX)]
        y_ticks = [i for i in range(0, len(pointY[0]), stepY)]
        labelX = [f"{i:.0f}" for i in pointX[0][::stepX]]
        labelY = [f"{i * 1000:.0f}" for i in pointY[0][::stepY]]

        pointZ = np.array(pointZ).reshape(len(pointY[0]), len(pointX[0]))
        max_index = np.unravel_index(np.argmax(pointZ), pointZ.shape)
        max_value = pointZ[max_index]
        pointX, pointY = np.meshgrid(pointX[0], pointY[0])
        """ _, ax = plt.subplots(subplot_kw={"projection": "3d"})
        surf = ax.plot_surface(pointX, pointY, pointZ, cmap="inferno")
        ax.set_xlabel("Частота, Гц")
        ax.set_ylabel("Временной сдвиг, с")
        ax.set_title("Взаимная функция неопределенности")"""

        _, ax = plt.subplots()
        import math

        logPointZ = [[10 * math.log(j) for j in i] for i in pointZ]
        im = ax.imshow(logPointZ, cmap="inferno")

        def format_coord(x, y):
            """Форматирование координат для статусной строки"""
            x_int = int(x + 0.5)
            y_int = int(y + 0.5)
            if 0 <= x_int < pointZ.shape[1] and 0 <= y_int < pointZ.shape[0]:
                real_x = pointX[0][x_int]
                real_y = pointY[y_int][0] * 1000
                value = pointZ[y_int, x_int]
                return f"Частота: {real_x:.1f} Гц, Сдвиг: {real_y:.1f} мс, Значение: {value:.3f}"
            else:
                return f"x={x:.1f}, y={y:.1f}"

        # Устанавливаем функцию форматирования координат
        ax.format_coord = format_coord

        ax.plot(
            max_index[1],
            max_index[0],
            "ro",  # красная точка
            markersize=10,  # размер маркера
            markeredgecolor="white",  # белая обводка
            markerfacecolor="red",  # заливка красная
            label=f"Максимум",
        )

        ax.annotate(
            f"Максимум",
            xy=(max_index[1], max_index[0]),
            xytext=(max_index[1] + 10, max_index[0] - 10),
            arrowprops=dict(arrowstyle="->", color="red"),
            fontsize=10,
            color="red",
        )

        contour_lines = ax.contour(logPointZ, colors="black", linewidths=1, alpha=0.7)
        ax.set_xlabel("Частота, Гц")
        ax.set_ylabel("Временной сдвиг, мс")
        ax.set_title("Взаимная функция неопределенности")
        ax.set_xticks(x_ticks)
        ax.set_yticks(y_ticks)
        ax.set_xticklabels(labelX)
        ax.set_yticklabels(labelY)
        plt.colorbar(im, label="Значение взаимной функции неопределенности")

        _, ax = plt.subplots()
        im = ax.imshow(pointZ, cmap="inferno")

        def format_coord(x, y):
            """Форматирование координат для статусной строки"""
            x_int = int(x + 0.5)
            y_int = int(y + 0.5)
            if 0 <= x_int < pointZ.shape[1] and 0 <= y_int < pointZ.shape[0]:
                real_x = pointX[0][x_int]
                real_y = pointY[y_int][0] * 1000
                value = pointZ[y_int, x_int]
                return f"Частота: {real_x:.1f} Гц, Сдвиг: {real_y:.1f} мс, Значение: {value:.3f}"
            else:
                return f"x={x:.1f}, y={y:.1f}"

        # Устанавливаем функцию форматирования координат
        ax.format_coord = format_coord
        ax.plot(
            max_index[1],
            max_index[0],
            "ro",  # красная точка
            markersize=10,  # размер маркера
            markeredgecolor="white",  # белая обводка
            markerfacecolor="red",  # заливка красная
            label=f"Максимум: {max_value:.2f}",
        )

        ax.annotate(
            f"Максимум: {max_value:.2f}",
            xy=(max_index[1], max_index[0]),
            xytext=(max_index[1] + 10, max_index[0] - 10),
            arrowprops=dict(arrowstyle="->", color="red"),
            fontsize=10,
            color="red",
        )
        ax.set_xlabel("Частота, Гц")
        ax.set_ylabel("Временной сдвиг, мс")
        ax.set_title("Взаимная функция неопределенности")
        ax.set_xticks(x_ticks)
        ax.set_yticks(y_ticks)
        ax.set_xticklabels(labelX)
        ax.set_yticklabels(labelY)
        plt.colorbar(im, label="Значение взаимной функции неопределенности")
        # fig.tight_layout()


def draw_graph(name_file):
    try:
        file = open("graphs/" + name_file, encoding="utf-8")
        blocks = file.read().split("separation ")
        if "3d" not in name_file:
            plt.figure()
        for i in range(len(blocks)):
            if "3d" not in name_file:
                plt.subplot(len(blocks), 1, i + 1)
                plt.subplots_adjust(hspace=0.9)
            setting = blocks[i].split("\n")
            prepare_data(setting)
        file.close()
    except FileNotFoundError:
        modal_window("Ошибка", "Не найден файл " + name_file)
    except PermissionError:
        modal_window("Ошибка", "Нет доступа к файлу " + name_file)


def draw_graphs(argv):
    if len(argv) == 1:
        name_files = [i for i in os.listdir("graphs/") if i.endswith(".txt")]
    else:
        name_files = argv[1:]
    for name_file in name_files:
        draw_graph(name_file)
    plt.show()


print(sys.argv)
draw_graphs(sys.argv)
