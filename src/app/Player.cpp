#include "Player.h"
#include <GL/freeglut.h>
#include <algorithm>
#include <cmath>
#include <cstdio>
#include <memory>
#include "Soundtrack.h"

// glDrawPixels reads the canvas memory directly as tightly packed RGB bytes.
static_assert(sizeof(Color) == 3, "Color must be exactly 3 bytes for glDrawPixels");

namespace {

// GLUT callbacks are plain functions, so the player keeps its state here.
struct PlayerState {
    Canvas* canvas = nullptr;
    RenderFrame render;
    float duration = 0, fps = 30;
    std::vector<float> chapters;
    std::unique_ptr<Soundtrack> music;
    float time = 0;           // current film time in seconds
    float renderedTime = -1;  // time the canvas currently shows
    bool playing = true;
    int lastTick = 0;         // milliseconds, from glutGet(GLUT_ELAPSED_TIME)
    int windowW = 0, windowH = 0;
};
PlayerState g;

void seek(float seconds) {
    // Snap to a whole frame so stepping and seeking land on the same frames
    g.time = std::round(std::clamp(seconds, 0.0f, g.duration) * g.fps) / g.fps;
}

// A jump made by the viewer: the music has to restart from the new place.
void jumpTo(float seconds) {
    seek(seconds);
    g.music->restart();
}

void updateTitle() {
    int now = static_cast<int>(g.time), total = static_cast<int>(g.duration);
    char title[96];
    std::snprintf(title, sizeof(title), "Behind the Smile   %d:%02d / %d:%02d%s", now / 60, now % 60, total / 60,
                  total % 60, g.playing ? "" : "   (paused)");
    glutSetWindowTitle(title);
}

void display() {
    if (g.time != g.renderedTime) {
        g.render(*g.canvas, g.time);
        g.renderedTime = g.time;
    }

    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT);

    // Scale the canvas to fit the window, keeping its shape, centred
    const int w = g.canvas->width(), h = g.canvas->height();
    float zoom = std::min(g.windowW / static_cast<float>(w), g.windowH / static_cast<float>(h));
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, g.windowW, 0, g.windowH, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glRasterPos2f((g.windowW - w * zoom) / 2, (g.windowH - h * zoom) / 2);
    glPixelZoom(zoom, zoom);
    glDrawPixels(w, h, GL_RGB, GL_UNSIGNED_BYTE, g.canvas->pixels().data());
    glutSwapBuffers();
}

void reshape(int w, int h) {
    g.windowW = w;
    g.windowH = h;
    glViewport(0, 0, w, h);
}

void tick(int) {
    if (glutGetWindow() == 0) return;  // the window has been closed
    int now = glutGet(GLUT_ELAPSED_TIME);
    if (g.playing) {
        // Advance by real elapsed time, so the film keeps its speed even if a frame is slow
        g.time = std::min(g.duration, g.time + (now - g.lastTick) / 1000.0f);
        if (g.time >= g.duration) g.playing = false;
        seek(g.time);
    }
    g.lastTick = now;
    g.music->update(g.time, g.playing);
    updateTitle();
    glutPostRedisplay();
    glutTimerFunc(static_cast<unsigned>(1000 / g.fps), tick, 0);
}

void keyboard(unsigned char key, int, int) {
    switch (key) {
        case ' ':
            if (g.time >= g.duration) seek(0);
            g.playing = !g.playing;
            g.music->restart();
            break;
        case 'r': case 'R': jumpTo(0); g.playing = true; break;
        case 'm': case 'M': g.music->setMuted(!g.music->muted()); break;
        case ',': g.playing = false; jumpTo(g.time - 1 / g.fps); break;
        case '.': g.playing = false; jumpTo(g.time + 1 / g.fps); break;
        case 27: glutLeaveMainLoop(); return;
        default:
            if (key >= '1' && key <= '9') {
                size_t chapter = key - '1';
                if (chapter < g.chapters.size()) jumpTo(g.chapters[chapter]);
            }
            break;
    }
    updateTitle();
    glutPostRedisplay();
}

void special(int key, int, int) {
    if (key == GLUT_KEY_LEFT) jumpTo(g.time - 5);
    if (key == GLUT_KEY_RIGHT) jumpTo(g.time + 5);
    updateTitle();
    glutPostRedisplay();
}

}  // namespace

void runPlayer(int& argc, char** argv, Canvas& canvas, float duration, float fps,
               const std::vector<float>& chapters, const std::vector<SoundCue>& sound, RenderFrame render) {
    g.canvas = &canvas;
    g.render = std::move(render);
    g.duration = duration;
    g.fps = fps;
    g.chapters = chapters;
    g.music = std::make_unique<Soundtrack>(sound);

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(canvas.width(), canvas.height());
    glutCreateWindow("Behind the Smile");
    glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(special);
    g.lastTick = glutGet(GLUT_ELAPSED_TIME);
    glutTimerFunc(0, tick, 0);
    glutMainLoop();
    g.music.reset();  // stops and closes the music
}
