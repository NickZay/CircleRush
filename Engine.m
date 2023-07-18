
//
//  DO NOT MODIFY THIS FILE
//


#include "Engine.h"
#import <Foundation/Foundation.h>
#import <Cocoa/Cocoa.h>
#include <assert.h>
#include <stdlib.h>
#include <Carbon/Carbon.h>
#include <mach/mach_time.h>

#if __MAC_OS_X_VERSION_MAX_ALLOWED < 101200
#define NSWindowStyleMaskTitled NSTitledWindowMask
#define NSEventMaskAny NSAnyEventMask
#endif

uint32_t buffer[SCREEN_HEIGHT][SCREEN_WIDTH];

static id window;
static float freq = 1.0;
static uint64_t prevTime = 0;
static int mouseX = 0;
static int mouseY = 0;
static bool quit = false;
static bool key_state[VK__COUNT] = {0};
static bool mb_state[3] = {0};

void schedule_quit_game(void)
{
  quit = true;
}

bool is_key_pressed(int button_vk_code)
{
  if ((unsigned)(button_vk_code) < VK__COUNT)
    return key_state[button_vk_code];
  return false;
}

bool is_mouse_button_pressed(int mouse_button)
{
  if ((unsigned)(mouse_button) < 2)
    return mb_state[mouse_button];
  return false;
}

int get_cursor_x()
{
  return mouseX;
}

int get_cursor_y()
{
  return mouseY;
}

static void set_key(int code, bool state)
{
  switch (code)
  {
    case kVK_Escape: key_state[VK_ESCAPE] = state; break;
    case kVK_Return: key_state[VK_RETURN] = state; break;
    case kVK_LeftArrow: key_state[VK_LEFT] = state; break;
    case kVK_RightArrow: key_state[VK_RIGHT] = state; break;
    case kVK_UpArrow: key_state[VK_UP] = state; break;
    case kVK_DownArrow: key_state[VK_DOWN] = state; break;
    case kVK_Space: key_state[VK_SPACE] = state; break;
    default: break;
  }
}

@interface GameNSWindow : NSWindow <NSWindowDelegate>
@property bool isActivated;
@property(assign) NSAutoreleasePool* autoreleasePool;
@end

@implementation GameNSWindow
- (id)initWithContentRect:(NSRect)contentRect styleMask:(NSWindowStyleMask)windowStyle backing:(NSBackingStoreType)backingType defer:(BOOL)deferCreation
{
  self = [super initWithContentRect:contentRect styleMask:windowStyle backing:backingType defer:deferCreation];
  _isActivated = false;
  return self;
}

- (void)quit
{
  quit = true;
}

- (void)keyDown:(NSEvent *) theEvent
{
  set_key(theEvent.keyCode, true);
}

- (void)keyUp:(NSEvent *) theEvent
{
  set_key(theEvent.keyCode, false);
}

- (void)mouseDown:(NSEvent *) theEvent
{
  mb_state[0] = true;
}

- (void)mouseUp:(NSEvent *) theEvent
{
  mb_state[0] = false;
}

- (void)rightMouseDown:(NSEvent *) theEvent
{
  mb_state[1] = true;
}

- (void)rightMouseUp:(NSEvent *) theEvent
{
  mb_state[1] = false;
}

- (BOOL)acceptsFirstResponder
{
  return YES;
}
@end

@interface GameNSView : NSView
{
  CGContextRef backingBitmapContext;
  CGColorSpaceRef colourSpace;
}

- (void)drawRect:(NSRect)dirtyRect;
@end

@implementation GameNSView
- (void)drawRect:(NSRect)dirtyRect
{
  assert(backingBitmapContext != 0);
  CGImageRef img = CGBitmapContextCreateImage(backingBitmapContext);
  CGContextRef currentContext = [[NSGraphicsContext currentContext] CGContext];
  assert(currentContext != 0);
  CGContextDrawImage(currentContext, CGRectMake(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT), img);
  CGImageRelease(img);
}

- (id)initWithFrame:(NSRect)frameRect //gameTemplateWindow:(Window*) inGameWindow
{
  self = [super initWithFrame:frameRect];
  [self createBackingBitmapContext];
  return self;
}

- (void)dealloc
{
  CGColorSpaceRelease(colourSpace);
  CGContextRelease(backingBitmapContext);
  [super dealloc];
}

- (void)createBackingBitmapContext
{
  colourSpace = CGColorSpaceCreateDeviceRGB();
  backingBitmapContext = CGBitmapContextCreate(&buffer[0][0], SCREEN_WIDTH, SCREEN_HEIGHT, 8, SCREEN_WIDTH * 4,
    colourSpace, kCGBitmapByteOrder32Little | kCGImageAlphaNoneSkipFirst);
  assert(backingBitmapContext != 0);
}

@end


static bool initialize_window(const char* title, int width, int height)
{
  NSAutoreleasePool* autoreleasePool = [NSAutoreleasePool new];
  [NSApplication sharedApplication];

  window = [[[GameNSWindow alloc] initWithContentRect:NSMakeRect(0, 0, width, height)
    styleMask:NSWindowStyleMaskTitled backing:NSBackingStoreBuffered defer:NO] autorelease];
  [window setDelegate:window];
  [window cascadeTopLeftFromPoint:NSMakePoint(16, 16)];
  [window setTitle:[NSString stringWithCString:title encoding:NSUTF8StringEncoding]];
  [window makeKeyAndOrderFront:window];
  [window setReleasedWhenClosed:TRUE];
  [window setAutoreleasePool:autoreleasePool];

  id menubar = [[NSMenu new] autorelease];
  id appMenuItem = [[NSMenuItem new] autorelease];
  [menubar addItem:appMenuItem];
  [NSApp setMainMenu:menubar];
  id appMenu = [[NSMenu new] autorelease];
  id appName = [[NSProcessInfo processInfo] processName];
  id quitTitle = [@"Quit " stringByAppendingString:appName];
  id quitMenuItem = [[[NSMenuItem alloc] initWithTitle:quitTitle action:@selector(quit) keyEquivalent:@"q"] autorelease];
  [quitMenuItem setTarget:window];
  [appMenu addItem:quitMenuItem];
  [appMenuItem setSubmenu:appMenu];

  GameNSView* view = [[[GameNSView alloc] initWithFrame:NSMakeRect(0, 0, width, height)] autorelease];

  [window setContentView:view];

  [NSApp setActivationPolicy:NSApplicationActivationPolicyRegular];
  [NSApp finishLaunching];

  NSGraphicsContext* graphicsContext = [[NSGraphicsContext graphicsContextWithWindow:window] autorelease];
  assert(graphicsContext != 0);
  [NSGraphicsContext setCurrentContext:graphicsContext];

  prevTime = mach_absolute_time();
  mach_timebase_info_data_t timebase;
  mach_timebase_info(&timebase);
  freq = (timebase.denom * 1e9) / timebase.numer;

  return true;
}


static bool update_window()
{
  NSPoint mousePos;
  mousePos = [window mouseLocationOutsideOfEventStream];
  mouseX = mousePos.x;
  mouseY = SCREEN_HEIGHT - mousePos.y - 1;

  uint64_t curTime = mach_absolute_time();
  uint64_t delta = curTime - prevTime;
  float dt = delta / freq;
  if (dt > 0.1)
    dt = 0.1;
  prevTime = curTime;

  NSAutoreleasePool* pool = [[NSAutoreleasePool alloc] init];

  NSEvent* event;
  while (nil != (event = [NSApp nextEventMatchingMask:NSEventMaskAny untilDate:[NSDate distantPast] inMode:NSDefaultRunLoopMode dequeue:YES]))
  {
    [NSApp sendEvent:event];
  }

  act(dt);
  if (!quit)
    draw();
  
  [[window contentView] setNeedsDisplay:TRUE];

  if (![window isActivated])
  {
    [NSApp activateIgnoringOtherApps:YES];
    [window setIsActivated:TRUE];
  }

  [pool release];

  return !quit;
}

static void close_window()
{
  NSAutoreleasePool* autoreleasePool = [window autoreleasePool];
  [autoreleasePool release];
}


int main(int argc, const char * argv[])
{
  initialize_window("Game", SCREEN_WIDTH, SCREEN_HEIGHT);

  initialize();
  for (;;)
  {
    if (!update_window())
      break;
    usleep(1000);
  }

  finalize();
  close_window();
  return 0;
}


