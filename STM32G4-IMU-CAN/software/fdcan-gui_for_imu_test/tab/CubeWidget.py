import sys
from PySide6.QtWidgets import QApplication, QVBoxLayout, QWidget
from PySide6.QtCore import Qt
from PySide6.QtOpenGLWidgets import QOpenGLWidget
from OpenGL.GL import *
from OpenGL.GLU import *

class CubeWidget(QOpenGLWidget):
    def __init__(self):
        super(CubeWidget, self).__init__()
        self.raw = 0
        self.yaw = 0
        self.pitch = 0
        self.scale_factor = 1 / 3  # 비율

        # color of each side
        self.front_color = (0.0, 0.0, 1.0)  
        self.back_color = (1.0, 0.0, 0.0)   
        self.left_color = (1.0, 1.0, 1.0)   
        self.right_color = (0.0, 1.0, 0.0)  
        self.top_color = (1.0, 1.0, 0.0)    
        self.bottom_color = (1.0, 0.5, 0.0) 

    def initializeGL(self):
        # set background color black
        glClearColor(0.0, 0.0, 0.0, 1.0) 
        # enable depth testing
        glEnable(GL_DEPTH_TEST)  

    def resizeGL(self, w, h):
        glViewport(0, 0, w, h)
        glMatrixMode(GL_PROJECTION)
        glLoadIdentity()
        # 시야각 45도, 종횡비, 가깝고 먼 클리핑 평면 설정??
        gluPerspective(45, w / h, 0.1, 100.0)  
        glMatrixMode(GL_MODELVIEW)

    def paintGL(self):
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)
        glLoadIdentity()

        # camera position
        glTranslatef(0.0, 0.0, -5.0)

        # rotation
        glRotatef(self.raw, 1.0, 0.0, 0.0)   # X axis(raw)
        glRotatef(self.yaw, 0.0, 1.0, 0.0)   # Y axis (yaw)
        glRotatef(self.pitch, 0.0, 0.0, 1.0) # Z axis (pitch)

        # draw box
        glBegin(GL_QUADS)
        scale = self.scale_factor 

        # front blue
        glColor3f(*self.front_color)
        glVertex3f(-1.0 * scale, -0.5 * scale, 1.0 * scale)
        glVertex3f(1.0 * scale,  -0.5 * scale, 1.0 * scale)
        glVertex3f(1.0 * scale,   0.5 * scale, 1.0 * scale)
        glVertex3f(-1.0 * scale,  0.5 * scale, 1.0 * scale)

        # back red
        glColor3f(*self.back_color)
        glVertex3f(-1.0 * scale, -0.5 * scale, -1.0 * scale)
        glVertex3f(1.0 * scale,  -0.5 * scale, -1.0 * scale)
        glVertex3f(1.0 * scale,   0.5 * scale, -1.0 * scale)
        glVertex3f(-1.0 * scale,  0.5 * scale, -1.0 * scale)

        # left white
        glColor3f(*self.left_color)
        glVertex3f(-1.0 * scale, -0.5 * scale, -1.0 * scale)
        glVertex3f(-1.0 * scale, -0.5 * scale,  1.0 * scale)
        glVertex3f(-1.0 * scale,  0.5 * scale,  1.0 * scale)
        glVertex3f(-1.0 * scale,  0.5 * scale, -1.0 * scale)

        # right green
        glColor3f(*self.right_color)
        glVertex3f(1.0 * scale, -0.5 * scale, -1.0 * scale)
        glVertex3f(1.0 * scale, -0.5 * scale,  1.0 * scale)
        glVertex3f(1.0 * scale,  0.5 * scale,  1.0 * scale)
        glVertex3f(1.0 * scale,  0.5 * scale, -1.0 * scale)

        # top yallo
        glColor3f(*self.top_color)
        glVertex3f(-1.0 * scale, 0.5 * scale, -1.0 * scale)
        glVertex3f(1.0 * scale,  0.5 * scale, -1.0 * scale)
        glVertex3f(1.0 * scale,  0.5 * scale,  1.0 * scale)
        glVertex3f(-1.0 * scale, 0.5 * scale,  1.0 * scale)

        # bottom orange
        glColor3f(*self.bottom_color)
        glVertex3f(-1.0 * scale, -0.5 * scale, -1.0 * scale)
        glVertex3f(1.0 * scale,  -0.5 * scale, -1.0 * scale)
        glVertex3f(1.0 * scale,  -0.5 * scale,  1.0 * scale)
        glVertex3f(-1.0 * scale, -0.5 * scale,  1.0 * scale)

        glEnd()

    '''
    0 ~ 360
    '''
    def setRoll(self, value):
        self.raw = value
        self.update()

    def setYaw(self, value):
        self.yaw = value
        self.update()

    def setPitch(self, value):
        self.pitch = value
        self.update()
    
    '''
    0.1 ~ 1.0
    '''
    def setScale(self, value):
        self.scale_factor = value
        self.update()


class CubeControlWidget(QWidget):
    def __init__(self, parent=None):
        super(CubeControlWidget, self).__init__(parent)
        self.cube_widget = CubeWidget()        
        layout = QVBoxLayout()
        layout.addWidget(self.cube_widget)
        self.setLayout(layout)


if __name__ == "__main__":
    app = QApplication(sys.argv)
    widget = CubeControlWidget()
    widget.show()
    sys.exit(app.exec())
