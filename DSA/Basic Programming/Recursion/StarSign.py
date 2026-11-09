import pygame
from pygame.locals import *
from OpenGL.GL import *
from OpenGL.GLU import *

pygame.init()

screen_width = 1000
screen_height = 800

screen = pygame.display.set_mode((screen_width, screen_height), DOUBLEBUF | OPENGL)
pygame.display.set_caption("OpenGL in Python")


def init_ortho():
    glMatrixMode(GL_PROJECTION)
    glLoadIdentity()
    gluOrtho2D(0, 1000, 0, 800)


done = False
init_ortho()


def draw_star(x, y, size):
    glPointSize(size)
    glBegin(GL_POINTS)
    glVertex2i(x, y)
    glEnd()


while not done:
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            done = True

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)
    glMatrixMode(GL_MODELVIEW)
    glLoadIdentity()

    draw_star(231, 151, 20)
    draw_star(257, 253, 20)
    draw_star(303, 180, 15)
    draw_star(443, 228, 20)
    draw_star(435, 287, 10)
    draw_star(385, 315, 20)
    draw_star(371, 343, 10)
    draw_star(397, 377, 10)
    draw_star(435, 373, 10)

    pygame.display.flip()
    pygame.time.wait(100)

pygame.quit()
