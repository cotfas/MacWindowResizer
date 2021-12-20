/* Carbon includes everything necessary for Accessibilty API */
#include <Carbon/Carbon.h>
#include <CoreGraphics/CGDisplayConfiguration.h>

//Inspiring source: https://stackoverflow.com/questions/614185/window-move-and-resize-apis-in-os-x

static bool amIAuthorized ()
{
    if (AXAPIEnabled() != 0) {
        /* Yehaa, all apps are authorized */
        return true;
    }
    /* Bummer, it's not activated, maybe we are trusted */
    if (AXIsProcessTrusted() != 0) {
        /* Good news, we are already trusted */
        return true;
    }
    /* Crap, we are not trusted...
     * correct behavior would now be to become a root process using
     * authorization services and then call AXMakeProcessTrusted() to make
     * ourselves trusted, then restart... I'll skip this here for
     * simplicity.
     */
    return false;
}


static AXUIElementRef getFrontMostApp ()
{
    pid_t pid;
    ProcessSerialNumber psn;

    GetFrontProcess(&psn);
    GetProcessPID(&psn, &pid);
    return AXUIElementCreateApplication(pid);
}

/*
compile with:
gcc -framework Carbon -o resizer resizer.c
*/
int main (
    int argc,
    char ** argv
) {


    AXValueRef temp;
    CGSize windowSize;
    CGPoint windowPosition;
    CFStringRef windowTitle;
    AXUIElementRef frontMostApp;
    AXUIElementRef frontMostWindow;

    if (!amIAuthorized()) {
        printf("Can't use accessibility API!\n");
        return 1;
    }


    /* Here we go. Find out which process is front-most */
    frontMostApp = getFrontMostApp();

    /* Get the front most window. We could also get an array of all windows
     * of this process and ask each window if it is front most, but that is
     * quite inefficient if we only need the front most window.
     */
    AXUIElementCopyAttributeValue(
        frontMostApp, kAXFocusedWindowAttribute, (CFTypeRef *)&frontMostWindow
    );

    /* Get the title of the window */
    AXUIElementCopyAttributeValue(
        frontMostWindow, kAXTitleAttribute, (CFTypeRef *)&windowTitle
    );

    /* Get the window size and position */
    AXUIElementCopyAttributeValue(
        frontMostWindow, kAXSizeAttribute, (CFTypeRef *)&temp
    );
    AXValueGetValue(temp, kAXValueCGSizeType, &windowSize);
    CFRelease(temp);

    AXUIElementCopyAttributeValue(
        frontMostWindow, kAXPositionAttribute, (CFTypeRef *)&temp
    );
    AXValueGetValue(temp, kAXValueCGPointType, &windowPosition);
    CFRelease(temp);

    /* Print everything */
    printf("\n");
    CFShow(windowTitle);
    printf(
        "Window is at (%f, %f) and has dimension of (%f, %f)\n",
        windowPosition.x,
        windowPosition.y,
        windowSize.width,
        windowSize.height
    );




    //getting display sizes
    auto mainDisplayId = CGDirectDisplayID();//? todo fix it when multiple monitors connected
    double screenWidth = CGDisplayPixelsWide(mainDisplayId);
    double screenHeight = CGDisplayPixelsHigh(mainDisplayId);
    printf(
        "Screen size is (%f, %f) \n",
        screenWidth,
        screenHeight
    );

    
    // init definitions
    double newPositionX = 0.0;
    double newPositionY = 0.0;
    
    double newWindowWidth = 0.0;
    double newWindowHeight = 0.0;

    bool executeMoving = true;
    bool executeResizing = true;
    
    
    if (argv != NULL) {
        //arguments
        double widthRatio = strtod(argv[1], NULL);
        double heightRatio = strtod(argv[2], NULL);
        printf(
               "Params widthRatio/heightRatio (%f, %f)\n",
               widthRatio,
               heightRatio
               );
        
        //for resize
        newWindowWidth = screenWidth * widthRatio;
        newWindowHeight = screenHeight * heightRatio;
        
        //double currentWindowWidth = windowSize.width;
        //double currentWindowHeight = windowSize.height;
        newPositionX = (screenWidth - newWindowWidth) / 2; //using future size instead of currentWindowWidth
        newPositionY = (screenHeight - newWindowHeight) / 2; //using future size instead of currentWindowHeight
    }
    
    if (executeMoving) {
        //exec centering
        windowPosition.x = newPositionX;
        windowPosition.y = newPositionY;
        temp = AXValueCreate(kAXValueCGPointType, &windowPosition);
        AXUIElementSetAttributeValue(frontMostWindow, kAXPositionAttribute, temp);
        CFRelease(temp);
    }


    if (executeResizing) {
        //exec resizing
        windowSize.width = newWindowWidth;
        windowSize.height = newWindowHeight;
        temp = AXValueCreate(kAXValueCGSizeType, &windowSize);
        AXUIElementSetAttributeValue(frontMostWindow, kAXSizeAttribute, temp);
        CFRelease(temp);
    }
   



    /* Clean up */
    CFRelease(frontMostWindow);
    CFRelease(frontMostApp);

    return 0;
}
