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
    plt.figure()
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
            pointX = [float(i) for i in value.split()]
            continue
        elif name == "pointY":
            pointY = [float(i) for i in value.split()]
            continue
    plt.plot(pointX, pointY)
    plt.grid(True)


def draw_graph(name_file):
    try:
        file = open("graphs/" + name_file, encoding="utf-8")
        setting = file.read().split("\n")
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
