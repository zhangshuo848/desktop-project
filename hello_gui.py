import tkinter as tk
from tkinter import messagebox

# 创建主窗口
window = tk.Tk()
window.title("Hello World GUI")
window.geometry("300x200")  # 窗口大小

# 添加标签
label = tk.Label(window, text="Hello World!", font=("Arial", 16))
label.pack(pady=30)  # 放置标签并设置边距

# 添加按钮（点击弹窗）
def show_message():
    messagebox.showinfo("Message", "Hello World from Dialog!")

button = tk.Button(window, text="Click Me", command=show_message)
button.pack()

# 启动事件循环
window.mainloop()