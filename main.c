#include <stdio.h>
#include <stdbool.h>

#include <SDL.h>
#include <SDL_image.h>

#include <gtk/gtk.h>
#include <gdk/gdkx.h>

// CONSTANTS
// Window dimensions
static const int width = 1200;
static const int height = 840;
static const int sdl_width = 840;
static const int squareLength = 40;
static const int FPS = 60;

// VARIABLES
// SDL variables
static SDL_Window *sdlWindow;
static SDL_Renderer *sdlRenderer;

// GTK variables
static GtkWindow *gtkWindow;
static GtkWidget *gtkHbox;
static GtkWidget *gtkDA;
static void *gdk_window;
static void *window_id;

GtkWidget *entry;

// Control variables
static int sprite = 0;
static int f_time = 0;

// SDL variables
static SDL_Texture *background;
static SDL_Rect backgroundRect;

// Function declaration
void destroy(GtkWidget* widget, gpointer data);
void greet(GtkWidget* widget, gpointer data);
static gboolean idle(void *ud);
SDL_Texture* loadImage(const char* file, SDL_Renderer *renderer);
gboolean onKeyPress(GtkWidget *widget, GdkEventKey *event, gpointer data);

void initSDL();

static int counter = 0;

int main(int argc, char** argv) {
    // Start SDL
    SDL_Init(SDL_INIT_VIDEO);

    // Main GTK window
    gtk_init(&argc, &argv);
    gtkWindow = (GtkWindow*)gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(gtkWindow, "GTK+SDL");
    gtk_window_set_default_size(gtkWindow, width, height);

    // GTK main boxes
    gtkHbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 680);
    GtkWidget *controlPanel = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_margin_end(controlPanel, 10);

    // GTK graphic elements
    gtkDA = gtk_drawing_area_new();
    GtkWidget *mainTitle = gtk_label_new("GTK+SDL");
    entry = gtk_entry_new();
    GtkWidget *button = gtk_button_new_with_label ("Button");

    PangoAttrList *mainTitleAttrs = pango_attr_list_new();
    pango_attr_list_insert(mainTitleAttrs, pango_attr_size_new(28 * PANGO_SCALE));
    gtk_label_set_attributes((GtkLabel*)mainTitle, mainTitleAttrs);
    gtk_widget_set_halign(mainTitle, GTK_ALIGN_CENTER);

    gtk_entry_set_placeholder_text((GtkEntry*) entry, "Enter text here");
    g_signal_connect(button, "clicked", G_CALLBACK (greet), "button");

    // Add elements in boxes
    gtk_container_add (GTK_CONTAINER(gtkWindow), gtkHbox);
    gtk_box_pack_start (GTK_BOX(gtkHbox), gtkDA, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX (gtkHbox), controlPanel, TRUE, TRUE, 0);

    // Control panel elements
    gtk_box_pack_start(GTK_BOX(controlPanel), mainTitle, FALSE, FALSE, 10);
    gtk_box_pack_start(GTK_BOX(controlPanel), entry, FALSE, FALSE, 10);
    gtk_box_pack_start (GTK_BOX(controlPanel), button, FALSE, FALSE, 10);

    gtk_widget_show_all ((GtkWidget*) gtkWindow);

    // Get window ID for SDL window creation
    gdk_window = gtk_widget_get_window(GTK_WIDGET(gtkDA));
    window_id = (void*)(intptr_t)GDK_WINDOW_XID(gdk_window);

    // GTK idle function control SDL loop
    g_idle_add(&idle, 0);

    // Handle program exit
    g_signal_connect(gtkWindow, "destroy", G_CALLBACK(destroy), NULL);
    gtk_container_set_border_width(GTK_CONTAINER(gtkWindow), 0);

    gtk_widget_add_events((GtkWidget *) gtkWindow, GDK_KEY_PRESS_MASK);
    g_signal_connect(G_OBJECT(gtkWindow), "key_press_event", G_CALLBACK(onKeyPress), NULL);

    // Start GTK
    gtk_main();

    return 0;
}

/**
 * Clears main GTK and SDL memory on program exit
 */
void destroy(GtkWidget* widget, gpointer data) {
    // Safe memory exit. Delete stuff
    g_print("Deleting stuff for safe exit\n");

    // Delete SDL stuff
    SDL_DestroyTexture(background);
    IMG_Quit();
    SDL_DestroyRenderer(sdlRenderer);
    SDL_DestroyWindow(sdlWindow);
    SDL_Quit();

    // Close GTK
    gtk_main_quit();
}

/**
 * Called when button is pressed
 */
void greet(GtkWidget* widget, gpointer data) {
    // Update counter and print
    g_print("%s clicked %d times\n", (char*)data, ++counter);

    // Read text from entry
    const gchar *entryText = gtk_entry_get_text(GTK_ENTRY(entry));
    g_print("Entry text is: %s\n", entryText);
}

/**
 * GTK loop function on idle
 */
static gboolean idle(void *ud) {
    (void)ud;
    if(!sdlWindow) {
        // Create window
        printf("Creating SDL window for window id %p\n", window_id);
        sdlWindow = SDL_CreateWindowFrom(window_id);
        SDL_SetWindowSize(sdlWindow, sdl_width, height);
        printf("sdl_window=%p\n", sdlWindow);
        if(!sdlWindow) {
            printf("%s\n", SDL_GetError());
        }

        // Create renderer
        sdlRenderer = SDL_CreateRenderer(sdlWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
        printf("sdl_renderer=%p\n", sdlRenderer);
        if(!sdlRenderer) {
            printf("%s\n", SDL_GetError());
        }
        IMG_Init(IMG_INIT_PNG);

        // Load images
        background = loadImage("../img/bg.png", sdlRenderer);

        backgroundRect = (SDL_Rect){0, 0, sdl_width, height};

    } else {
        // Main SDL loop
        SDL_RenderClear(sdlRenderer);
        SDL_RenderCopy(sdlRenderer, background, NULL, &backgroundRect);

        SDL_RenderPresent(sdlRenderer);

        // Handle frame on loop
        f_time++;
        if (FPS / f_time == 4) {
            f_time = 0;
            sprite++;
        }
        SDL_Delay(1000/120);
    }
    return true;
}

/**
 * Create SDL texture from image file
 * @param file path to image JPG or PNG
 * @param renderer SDL renderer
 * @return SDL texture
 */
SDL_Texture* loadImage(const char* file, SDL_Renderer* renderer) {
    // Load image as surface
    SDL_Surface* image = IMG_Load(file);

    // Verify file is correctly found
    if (image == NULL ){
        printf( "Unable to load image. SDL_image Error: %s\n", IMG_GetError());
    }

    // Convert to texture and delete surface
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, image);
    SDL_FreeSurface(image);
    return texture;
}

/**
 * Handles key presses using GTK
 * @return Boolean if key detection was successful
 */
gboolean onKeyPress (GtkWidget *widget, GdkEventKey *event, gpointer data) {
    gboolean result = TRUE;
    switch (event->keyval) {
        case GDK_KEY_Up:
            printf("Up key press\n");
            break;
        case GDK_KEY_Down:
            printf("Down key press\n");
            break;
        case GDK_KEY_Left:
            printf("Left key press\n");
            break;
        case GDK_KEY_Right:
            printf("Right key press\n");
            break;
        default:
            result = FALSE;
    }
    return result;
}
