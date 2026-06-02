#include "offscreen_render.h"

RefTexture::RefTexture(int width, int height) :
    mTexture(QOpenGLTexture::Target2D),
    mRefCount(0)
{
    mTexture.create();
    mTextureID =mTexture.textureId();
    mTexture.setMinMagFilters(QOpenGLTexture::Linear,QOpenGLTexture::Linear);
    mTexture.setWrapMode(QOpenGLTexture::ClampToBorder);
    mTexture.setBorderColor(QColor(1.0f,1.0f,1.0f,1.0f));
    mTexture.setFormat(QOpenGLTexture::RGBA8_UNorm);
    mTexture.setSize(width,height,24);
    mTexture.allocateStorage(QOpenGLTexture::RGB,QOpenGLTexture::UInt8);
}
//    GL函数实现
//{
//    auto functions = QOpenGLContext::currentContext()->functions();
//    functions->glGenTextures(1, &mTextureID);
//    functions->glActiveTexture(GL_TEXTURE0);
//    functions->glBindTexture(GL_TEXTURE_2D, mTextureID);
//    functions->glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//    functions->glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//    functions->glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
//    functions->glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
//    functions->glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
//    functions->glBindTexture(GL_TEXTURE_2D, 0);
//}

RefTexture::~RefTexture() {
    QOpenGLContext::currentContext()->functions()->glDeleteTextures(1, &mTextureID);
}

void RefTexture::retain() {
    mRefCount.ref();
}

void RefTexture::release() {
    if (!mRefCount.deref())
        delete this;
}

GLuint RefTexture::value() const {
    return mTextureID;
}


GLWidgetRenderer::GLWidgetRenderer(GLWidget * player) :
    mOutputTexture(nullptr),
    mInit(false),
    mIsRenderingFrame(false),
    mContext(nullptr),
    mGLWidget(player),
    mExiting(false)
{
    auto context=QOpenGLContext::currentContext();
    QSurfaceFormat format;
//    //设置像素位深度
//    format.setDepthBufferSize(24);
//    //设置R G B 通道位宽
//    format.setRedBufferSize(8);
//    format.setGreenBufferSize(8);
//    format.setBlueBufferSize(8);
//    //设置模板 alpha位宽
//    format.setStencilBufferSize(8);
//    format.setAlphaBufferSize(8);
//    //设置使用OpenGL版本和对应配置文件
//    format.setVersion(4, 5);
//    format.setProfile(QSurfaceFormat::CoreProfile);
    format = context->format();
    mSurface = new QOffscreenSurface();
    mSurface->setFormat(format);
    mSurface->create();
}

RefTexture* GLWidgetRenderer::getWriteTexture(int width, int height) {
    QMutexLocker lock(&mTextureLock);
    if (mOutputTexture)	{
        mOutputTexture->release();
    }
    mOutputTexture = new RefTexture(width, height);
    return mOutputTexture;
}

RefTexture* GLWidgetRenderer::grabTexture() {
    QMutexLocker lock(&mTextureLock);
    if (isRendering() || mOutputTexture == nullptr) {
        return nullptr;
    }
    else {
        mOutputTexture->retain();
        return mOutputTexture;
    }
}

void GLWidgetRenderer::setGLContext(QOpenGLContext * context, QThread * thread) {
    mContext = context;
    mThread = thread;
}

void GLWidgetRenderer::setRendering(bool isRendering) {
    QMutexLocker lock(&mRenderStateMutex);
    mIsRenderingFrame = isRendering;
}

bool GLWidgetRenderer::isRendering() {
    QMutexLocker lock(&mRenderStateMutex);
    return mIsRenderingFrame;
}

void GLWidgetRenderer::render() {
    if (mExiting) { return; }
    if (!mContext) { return; }

    QMutexLocker lock(&mRenderMutex);
    if (mExiting) { return; }

    mContext->makeCurrent(mSurface);
    if (!mInit) {
        mInit = true;
        initializeOpenGLFunctions();
        // 准备其他GL资源如: program,vao
    }

    setRendering(true);

    if (mOutputTexture == nullptr) {
        mOutputTexture = getWriteTexture(mGLWidget->sceneWidth(), mGLWidget->sceneHeight());
        // TODO:将outputTexture绑定到当前的FBO上面，用于存储渲染内容
    }
    emit frameRenderStart();
    // 渲染内容
    glFinish();
    emit frameRenderFinished();

    mContext->doneCurrent();
    setRendering(false);
    QMetaObject::invokeMethod(mGLWidget, "update");
}

void GLWidgetRenderer::stopRenderer() {
    // 销毁GL资源
}

GLWidget::GLWidget(int sceneWidth, int sceneHeight, QWidget *parent) :
    QOpenGLWidget(parent),
    mSceneWidth(sceneWidth),
    mSceneHeight(sceneHeight),
    mWidth(0),
    mHeight(0),
    mOutputTexture(nullptr)
{
    QSurfaceFormat format;
    format.setDepthBufferSize(24);
    format.setStencilBufferSize(8);
    format.setRedBufferSize(8);
    format.setBlueBufferSize(8);
    format.setGreenBufferSize(8);
    format.setAlphaBufferSize(8);
    format.setVersion(2, 0);
    format.setProfile(QSurfaceFormat::CoreProfile);
    setFormat(format);

    mThread = new QThread;
    mRenderer = new GLWidgetRenderer(this);
    mRenderer->moveToThread(mThread);
    connect(mThread, &QThread::finished, mRenderer, &QObject::deleteLater);
    connect(this, &GLWidget::renderRequested, mRenderer, &GLWidgetRenderer::render);
    connect(mRenderer, &GLWidgetRenderer::frameRenderStart, this, &GLWidget::startRenderFrame);
    connect(mRenderer, &GLWidgetRenderer::frameRenderFinished, this, &GLWidget::finishedRenderFrame);
    mThread->start();
}

GLWidget::~GLWidget() {
    stopRendering();
    mRenderer->prepareExit();
    mThread->quit();
    mThread->wait();
    delete mThread;
    makeCurrent();

}

int GLWidget::sceneWidth() const{
    return mSceneWidth;
}

int GLWidget::sceneHeight() const{
    return mSceneHeight;
}

void GLWidget::initializeGL() {
    initializeOpenGLFunctions();
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &mScreenFramebuffer);
    // 准备UI线程上的GL资源
    doneCurrent();

    QOpenGLContext * renderCtx = new QOpenGLContext();
    QSurfaceFormat format;
    format.setDepthBufferSize(24);
    format.setStencilBufferSize(8);
    format.setRedBufferSize(8);
    format.setBlueBufferSize(8);
    format.setGreenBufferSize(8);
    format.setAlphaBufferSize(8);
    format.setVersion(2, 0);
    format.setProfile(QSurfaceFormat::CoreProfile);
    renderCtx->setFormat(format);
    renderCtx->setShareContext(context());
    renderCtx->create();
    renderCtx->moveToThread(mThread);
    mRenderer->setGLContext(renderCtx, mThread);
}

void GLWidget::resizeGL(int w, int h) {
    mWidth = w * QApplication::desktop()->devicePixelRatio();
    mHeight = h * QApplication::desktop()->devicePixelRatio();
}

void GLWidget::startRenderFrame() {
    emit playFrameBegin();
}

void GLWidget::finishedRenderFrame() {
    emit playFrameFinished();
}

void GLWidget::paintGL() {
    mRenderer->lockRenderer();
    auto processTexture = mRenderer->grabTexture();
//    makeCurrent();
    glViewport(0, 0, mWidth, mHeight);
    glClearColor(0, 0, 0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);
    if (processTexture != nullptr || mOutputTexture != nullptr) {
        if (processTexture != nullptr) {
            if (mOutputTexture != nullptr) {
                mOutputTexture->release();
            }
            mOutputTexture = processTexture;
        }
        // 将mOutputTexture绘制到主FBO上用于显示
    }
//    doneCurrent();
    mRenderer->unlockRenderer();
}

void GLWidget::stopRendering() {
    mRenderer->lockRenderer();
    makeCurrent();
    mRenderer->stopRenderer();
    // 删除UI线程上的GL资源
    if (mOutputTexture) {
        mOutputTexture->release();
        mOutputTexture = nullptr;
    }
    doneCurrent();
    mRenderer->unlockRenderer();
    emit playFinished();
}

void GLWidget::refreshFrame() {
    if (!mRenderer->isRendering())
        emit renderRequested();
}
