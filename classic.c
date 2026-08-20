#include <gtk/gtk.h>
#include <vte/vte.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

GtkWidget *terminal;
GtkWidget *window;
double font_size = 12.0;
char current_font[128] = "Monospace 12";
char current_theme[64] = "classic";

// Wczytaj konfigurację
char* read_config(const char *file, const char *default_val) {
    char path[256];
    snprintf(path, sizeof(path), "%s/.%s", getenv("HOME"), file);
    FILE *f = fopen(path, "r");
    if (!f) return strdup(default_val);
    char *val = malloc(128);
    if (fgets(val, 128, f) == NULL) {
        free(val);
        fclose(f);
        return strdup(default_val);
    }
    val[strcspn(val, "\n")] = 0;
    fclose(f);
    return val;
}

void apply_font() {
    char font_str[256];
    snprintf(font_str, sizeof(font_str), "%s", current_font);
    PangoFontDescription *font_desc = pango_font_description_from_string(font_str);
    if (font_desc) {
        vte_terminal_set_font(VTE_TERMINAL(terminal), font_desc);
        pango_font_description_free(font_desc);
    }
}

void apply_theme() {
    GdkRGBA fg_color, bg_color;
    
    if (strcmp(current_theme, "dark") == 0) {
        gdk_rgba_parse(&fg_color, "#00ff00");
        gdk_rgba_parse(&bg_color, "#1e1e1e");
    } else if (strcmp(current_theme, "light") == 0) {
        gdk_rgba_parse(&fg_color, "#000000");
        gdk_rgba_parse(&bg_color, "#ffffff");
    } else if (strcmp(current_theme, "matrix") == 0) {
        gdk_rgba_parse(&fg_color, "#33ff33");
        gdk_rgba_parse(&bg_color, "#000000");
    } else if (strcmp(current_theme, "amber") == 0) {
        gdk_rgba_parse(&fg_color, "#ffb000");
        gdk_rgba_parse(&bg_color, "#000000");
    } else if (strcmp(current_theme, "white") == 0) {
        gdk_rgba_parse(&fg_color, "#ffffff");
        gdk_rgba_parse(&bg_color, "#000000");
    } else { // classic
        gdk_rgba_parse(&fg_color, "#00ff00");
        gdk_rgba_parse(&bg_color, "#000000");
    }
    
    vte_terminal_set_colors(VTE_TERMINAL(terminal), &fg_color, &bg_color, NULL, 0);
}

void zoom_in() {
    if (font_size < 24) {
        font_size += 1.0;
        char font_str[256];
        snprintf(font_str, sizeof(font_str), "Monospace %d", (int)font_size);
        strcpy(current_font, font_str);
        apply_font();
    }
}

void zoom_out() {
    if (font_size > 6) {
        font_size -= 1.0;
        char font_str[256];
        snprintf(font_str, sizeof(font_str), "Monospace %d", (int)font_size);
        strcpy(current_font, font_str);
        apply_font();
    }
}

// KOPIOWANIE PRZEZ GTK - BEZ OSTRZEŻEŃ!
void copy_text() {
    GtkClipboard *clipboard = gtk_clipboard_get(GDK_SELECTION_CLIPBOARD);
    vte_terminal_copy_clipboard(VTE_TERMINAL(terminal));
}

void paste_text() {
    GtkClipboard *clipboard = gtk_clipboard_get(GDK_SELECTION_CLIPBOARD);
    vte_terminal_paste_clipboard(VTE_TERMINAL(terminal));
}

gboolean on_key_press(GtkWidget *widget, GdkEventKey *event, gpointer data) {
    // Ctrl+Shift+C = Kopiuj
    if (event->keyval == GDK_KEY_C && 
        (event->state & GDK_CONTROL_MASK) && 
        (event->state & GDK_SHIFT_MASK)) {
        vte_terminal_copy_clipboard(VTE_TERMINAL(terminal));
        return TRUE;
    }
    
    // Ctrl+Shift+V = Wklej
    if (event->keyval == GDK_KEY_V && 
        (event->state & GDK_CONTROL_MASK) && 
        (event->state & GDK_SHIFT_MASK)) {
        vte_terminal_paste_clipboard(VTE_TERMINAL(terminal));
        return TRUE;
    }
    
    // Ctrl+Shift++ = Zoom in
    if (event->keyval == GDK_KEY_plus && 
        (event->state & GDK_CONTROL_MASK) && 
        (event->state & GDK_SHIFT_MASK)) {
        zoom_in();
        return TRUE;
    }
    
    // Ctrl+Shift+- = Zoom out
    if (event->keyval == GDK_KEY_minus && 
        (event->state & GDK_CONTROL_MASK) && 
        (event->state & GDK_SHIFT_MASK)) {
        zoom_out();
        return TRUE;
    }
    
    // Ctrl+0 = Reset zoom
    if (event->keyval == GDK_KEY_0 && 
        (event->state & GDK_CONTROL_MASK)) {
        font_size = 12.0;
        char font_str[256];
        snprintf(font_str, sizeof(font_str), "Monospace 12");
        strcpy(current_font, font_str);
        apply_font();
        return TRUE;
    }
    
    return FALSE;
}

int main(int argc, char *argv[]) {
    GtkWidget *vbox, *scrolled_window;
    char tmux_cmd[512];
    
    gtk_init(&argc, &argv);
    
    // Wczytaj konfigurację
    char *theme = read_config("classic_theme", "classic");
    char *font = read_config("classic_font", "Monospace 12");
    
    strcpy(current_theme, theme);
    strcpy(current_font, font);
    
    // Wyciągnij rozmiar z fontu
    char font_name[128];
    int size;
    sscanf(font, "%s %d", font_name, &size);
    font_size = size;
    if (font_size < 6) font_size = 12;
    if (font_size > 30) font_size = 12;
    
    free(theme);
    free(font);
    
    // OKNO
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Classic Terminal");
    gtk_window_set_default_size(GTK_WINDOW(window), 1000, 700);
    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    
    vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_container_add(GTK_CONTAINER(window), vbox);
    
    scrolled_window = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window),
                                   GTK_POLICY_AUTOMATIC,
                                   GTK_POLICY_AUTOMATIC);
    gtk_box_pack_start(GTK_BOX(vbox), scrolled_window, TRUE, TRUE, 0);
    
    terminal = vte_terminal_new();
    
    // Zastosuj motyw i font
    apply_theme();
    apply_font();
    
    vte_terminal_set_scrollback_lines(VTE_TERMINAL(terminal), 10000);
    
    gtk_container_add(GTK_CONTAINER(scrolled_window), terminal);
    g_signal_connect(terminal, "key-press-event", G_CALLBACK(on_key_press), NULL);
    
    // Spawn bash z tmux
    char *shell = getenv("SHELL");
    if (!shell) shell = "/bin/bash";
    
    char session_id[64];
    snprintf(session_id, sizeof(session_id), "classic_%d_%ld", getpid(), time(NULL));
    
    snprintf(tmux_cmd, sizeof(tmux_cmd), 
             "if command -v tmux >/dev/null 2>&1; then exec tmux new-session -s %s; else exec bash; fi",
             session_id);
    
    char *argv_cmd[] = {
        shell,
        "-c",
        tmux_cmd,
        NULL
    };
    
    vte_terminal_spawn_async(
        VTE_TERMINAL(terminal),
        VTE_PTY_DEFAULT,
        NULL,
        argv_cmd,
        NULL,
        0,
        NULL,
        NULL,
        NULL,
        -1,
        NULL,
        NULL,
        NULL
    );
    
    gtk_widget_show_all(window);
    gtk_widget_grab_focus(terminal);
    
    gtk_main();
    return 0;
}