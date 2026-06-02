#ifndef OFFSCREEN_RENDER_H
#define OFFSCREEN_RENDER_H

#pragma once

#include <QtWidgets>

class RefTexture{

private:
   ~RefTexture();

public:
   RefTexture(int width, int height);
   void retain();
   void release();
   GLuint value() const;
   GLuint mTextureID;
   QOpenGLTexture mTexture;
   QAtomicInt mRefCount;
};

class GLWidget;
class GLWidgetRenderer :public QObject, public QOpenGLFunctions {
    friend class GLWidget;
    Q_OBJECT
public:
   GLWidgetRenderer(GLWidget * player);
   void lockRenderer() { mRenderMutex.lock(); }
   void unlockRenderer() { mRenderMutex.unlock(); }
   void prepareExit() { mExiting = true; }
   void setGLContext(QOpenGLContext * context, QThread * thread);
   void stopRenderer();
   RefTexture * grabTexture();
public slots:
   void render();
Q_SIGNALS:
   void frameRenderStart();
   void frameRenderFinished();

private:
   RefTexture * getWriteTexture(int width, int height);
   void setRendering(bool isRendering);
   bool isRendering();
   RefTexture * mOutputTexture;
   QMutex mTextureLock;
   bool mInit;
   QMutex mRenderStateMutex;
   bool mIsRenderingFrame;
   QThread *mThread;
   QOpenGLContext * mContext;
   QOffscreenSurface * mSurface;
   GLWidget *mGLWidget;
   QMutex mRenderMutex;
   bool mExiting;
};

class GLWidget :public QOpenGLWidget, protected QOpenGLFunctions
{
   Q_OBJECT
private:
   int mSceneWidth, mSceneHeight;
   int mWidth, mHeight;

   QThread *mThread;
   GLWidgetRenderer *mRenderer;
   RefTexture * mOutputTexture;
   GLint mScreenFramebuffer;
   QOpenGLBuffer * mVbo;
   QOpenGLShaderProgram * mScreenProgram;
   void prepareGLResource();

public:
   explicit GLWidget(int sceneWidth, int sceneHeight, QWidget *parent = 0);
   virtual ~GLWidget();
   void refreshFrame();
   void stopRendering();
   int sceneWidth() const;
   int sceneHeight() const;

signals:
   void renderRequested();

Q_SIGNALS:
   void playFinished();
   void playFrameBegin();
   void playFrameFinished();

private slots:
   void startRenderFrame();
   void finishedRenderFrame();

protected:
   void initializeGL() override;
   void resizeGL(int w, int h) override;
   void paintGL() override;
};

#endif // OFFSCREEN_RENDER_H
