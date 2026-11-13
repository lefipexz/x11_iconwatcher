#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int x11_error_handler(Display *display, XErrorEvent *error)
{
    if (error->error_code == BadWindow)
        return 0;

    return 0;
}

#define MAX_PROCESSED 256

typedef struct {
    Window window;
} ProcessedWindow;

static ProcessedWindow processed[MAX_PROCESSED];
static int processed_count = 0;
static Window spotify_own_change = None;

/*
 * Verifica se já aplicamos o ícone nesta janela.
 */
static int already_processed(Window window)
{
    for (int i = 0; i < processed_count; i++) {
        if (processed[i].window == window)
            return 1;
    }

    return 0;
}


/*
 * Registra uma janela como processada.
 */
static void mark_processed(Window window)
{
    if (processed_count < MAX_PROCESSED) {
        processed[processed_count].window = window;
        processed_count++;
    }
}


/*
 * Executa o nosso seticon.
 */
static void apply_icon(Window window, const char *icon)
{
    char command[1024];

    snprintf(
        command,
        sizeof(command),
        "/home/lefipe/seticon 0x%lx \"%s\"",
        window,
        icon
    );

    printf(
        "[iconwatcher] Aplicando ícone → Window ID: 0x%lx\n",
        window
    );

    printf(
        "[iconwatcher] Comando: %s\n",
        command
    );

    fflush(stdout);

    int result = system(command);

    if (result != 0) {
        printf(
            "[iconwatcher] ERRO: seticon retornou %d\n",
            result
        );

        fflush(stdout);
    }
}


/*
 * Verifica uma janela individual.
 */
static void check_window(Display *display, Window window)
{
    Atom wm_class = XInternAtom(
        display,
        "WM_CLASS",
        False
    );

    Atom actual_type;
    int actual_format;
    unsigned long nitems;
    unsigned long bytes_after;
    unsigned char *data = NULL;

    if (XGetWindowProperty(
            display,
            window,
            wm_class,
            0,
            1024,
            False,
            XA_STRING,
            &actual_type,
            &actual_format,
            &nitems,
            &bytes_after,
            &data
        ) != Success)
        return;

    if (!data)
        return;

    char *instance = (char *)data;

    char *class = instance + strlen(instance) + 1;

    (void)class;

    const char *icon = NULL;
    const char *name = NULL;

    if (strcmp(instance, "chromium") == 0) {
        name = "Chromium";
        icon = "/home/lefipe/.local/share/icons/chromium-yamis.png";
    }

    else if (strcmp(instance, "discord") == 0) {
        name = "Discord";
        icon = "/home/lefipe/.local/share/icons/discord-yamis.png";
    }

    else if (strcmp(instance, "krita") == 0) {
        name = "Krita";
        icon = "/home/lefipe/.local/share/icons/krita-yamis.png";
    }

    else if (strcmp(instance, "gimp") == 0) {
        name = "GIMP";
        icon = "/home/lefipe/.local/share/icons/gimp-yamis.png";
    }

else if (strcmp(instance, "org.inkscape.Inkscape") == 0) {
    name = "Inkscape";
    icon = "/home/lefipe/.local/share/icons/inkscape-yamis.png";
}

    else if (strcmp(instance, "Alacritty") == 0) {
        name = "Alacritty";
        icon = "/home/lefipe/.local/share/icons/alacritty-yamis.png";
    }

    else if (strcmp(instance, "appimagekit-joplin") == 0) {
        name = "Joplin";
        icon = "/home/lefipe/.local/share/icons/joplin-yamis.png";
    }

    else if (strcmp(instance, "XnView") == 0) {
        name = "XnView MP";
        icon = "/home/lefipe/.local/share/icons/xnview-yamis.png";
    }

    else if (strcmp(instance, "audacity") == 0) {
        name = "Audacity";
        icon = "/home/lefipe/.local/share/icons/audacity-yamis.png";
    }

    else if (strcmp(instance, "kdenlive") == 0) {
        name = "Kdenlive";
        icon = "/home/lefipe/.local/share/icons/kdenlive-yamis.png";
    }

    else if (strcmp(instance, "steamwebhelper") == 0) {
        name = "Steam";
        icon = "/home/lefipe/.local/share/icons/steam-yamis.png";
    }

    else if (strcmp(instance, "Navigator") ==0) {
        name = "Firefox";
        icon = "/home/lefipe/.local/share/icons/firefox.alt.png";
    }

    else if (strcmp(instance, "rawtherapee") == 0) {
        name = "RawTherapee";
        icon = "/home/lefipe/.local/share/icons/darktable.png";
    }

else if (strcmp(instance, "spotify") == 0) {
    name = "Spotify";
    icon = "/home/lefipe/.local/share/icons/spotify-yamis.png";

    XSelectInput(display, window, PropertyChangeMask);
}

    if (name != NULL) {

    /*
     * Só processa e informa uma janela uma vez por Window ID.
     */
    if (!already_processed(window)) {

        printf(
            "[iconwatcher] %s encontrado → Window ID: 0x%lx\n",
            name,
            window
        );

        fflush(stdout);

        apply_icon(window, icon);

        mark_processed(window);
    }
}

    XFree(data);
}


/*
 * Lê todas as janelas presentes em _NET_CLIENT_LIST.
 */
static void scan_windows(Display *display, Window root)
{
    Atom client_list = XInternAtom(
        display,
        "_NET_CLIENT_LIST",
        False
    );

Atom net_wm_icon = XInternAtom(
    display,
    "_NET_WM_ICON",
    False
);

    Atom actual_type;
    int actual_format;
    unsigned long nitems;
    unsigned long bytes_after;
    unsigned char *data = NULL;

    if (XGetWindowProperty(
            display,
            root,
            client_list,
            0,
            4096,
            False,
            XA_WINDOW,
            &actual_type,
            &actual_format,
            &nitems,
            &bytes_after,
            &data
        ) != Success)
        return;

    if (!data)
        return;

    Window *windows = (Window *)data;

    for (unsigned long i = 0; i < nitems; i++)
        check_window(display, windows[i]);

    XFree(data);
}


int main(void)
{
    Display *display = XOpenDisplay(NULL);

XSetErrorHandler(x11_error_handler);

    if (!display) {
        fprintf(
            stderr,
            "[iconwatcher] Não foi possível conectar ao X11.\n"
        );

        return 1;
    }

    Window root = DefaultRootWindow(display);

    Atom client_list = XInternAtom(
        display,
        "_NET_CLIENT_LIST",
        False
    );

Atom net_wm_icon = XInternAtom(
    display,
    "_NET_WM_ICON",
    False
);

    XSelectInput(
        display,
        root,
        PropertyChangeMask
    );

    printf(
        "[iconwatcher] Iniciado.\n"
    );

    printf(
        "[iconwatcher] Verificando janelas existentes...\n\n"
    );

    fflush(stdout);

    /*
     * Primeira varredura.
     */
    scan_windows(display, root);

    printf(
        "\n[iconwatcher] Monitorando novas alterações...\n"
    );

    fflush(stdout);

    while (1) {

        XEvent event;

XNextEvent(display, &event);

if (event.type == PropertyNotify) {

    /*
     * _NET_CLIENT_LIST mudou:
     * pode ter surgido ou desaparecido uma janela.
     */
    if (event.xproperty.window == root &&
        event.xproperty.atom == client_list) {

        scan_windows(display, root);
    }

    /*
     * Uma janela mudou seu _NET_WM_ICON.
     * Se for Spotify, reaplicamos nosso ícone.
     */
    else if (event.xproperty.atom == net_wm_icon) {

        printf(
            "[iconwatcher] _NET_WM_ICON mudou → Window ID: 0x%lx\n",
            event.xproperty.window
        );

        fflush(stdout);

        /*
         * Procuramos essa janela para confirmar
         * que ela pertence ao Spotify.
         */
        XClassHint class_hint;

        if (XGetClassHint(
                display,
                event.xproperty.window,
                &class_hint
            )) {

            if (class_hint.res_name != NULL &&
    strcmp(class_hint.res_name, "spotify") == 0) {

    /*
     * Ignora a alteração provocada pelo nosso próprio seticon.
     */
    if (spotify_own_change == event.xproperty.window) {

        spotify_own_change = None;

    } else {

        printf(
            "[iconwatcher] Spotify alterou o ícone → reaplicando YAMIS.\n"
        );

        fflush(stdout);

        spotify_own_change = event.xproperty.window;

        apply_icon(
            event.xproperty.window,
            "/home/lefipe/.local/share/icons/spotify-yamis.png"
        );
    }
}

            if (class_hint.res_name != NULL)
                XFree(class_hint.res_name);

            if (class_hint.res_class != NULL)
                XFree(class_hint.res_class);
        }
    }
}
}

    XCloseDisplay(display);

    return 0;
}
