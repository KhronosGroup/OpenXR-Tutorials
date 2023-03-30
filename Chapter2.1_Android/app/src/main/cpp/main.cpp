#include "android_native_app_glue.h"
#include <jni.h>
#include <iostream>


void handle_cmd(android_app *pApp, int32_t cmd)
{
}

void android_main(struct android_app* app)
{
    app->onAppCmd = handle_cmd;

    int events;
    android_poll_source *pSource;
    do
    {
        if (ALooper_pollAll(0, nullptr, &events, (void **) &pSource) >= 0)
        {
            if (pSource)
            {
                pSource->process(app, pSource);
            }
        }
	    std::cout << "Hello from Android C++" << std::endl;
    }
    while (!app->destroyRequested);

}