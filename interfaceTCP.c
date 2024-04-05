#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

#define PORT 8080

GtkWidget *text_view;
GtkWidget *entry_username;
GtkWidget *entry_password;
GtkWidget *window;
GtkWidget *entry_choice;
GtkWidget *entry_filename;



int client_socket;





void printMenuToTextView() {
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_view));
    gtk_text_buffer_set_text(buffer, "           ------  LES SERVICES DISPONIBLES  ------\n \n", -1);
    gtk_text_buffer_insert_at_cursor(buffer, "1. Obtenir la date et l'heure\n", -1);
    gtk_text_buffer_insert_at_cursor(buffer, "2. Liste des fichiers du répertoire\n", -1);
    gtk_text_buffer_insert_at_cursor(buffer, "3. Le contenu d'un fichier\n", -1);
    gtk_text_buffer_insert_at_cursor(buffer, "4. La durée écoulée depuis la connexion\n", -1);
    gtk_text_buffer_insert_at_cursor(buffer, "0. Quitter\n", -1);
}





int authenticate(const char *username, const char *password) {
    char storedUsername[] = "abdoahmed";
    char storedPassword[] = "abdoahmed";

    if (strcmp(username, storedUsername) == 0 && strcmp(password, storedPassword) == 0) {
        printf("Authentification réussie.\n");
        return 1;
    } else {
        printf("Échec de l'authentification.\n");
        return 0;
    }
}





static gboolean on_button_clicked(GtkWidget *widget, gpointer data) {
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_view));

    const char *username = gtk_entry_get_text(GTK_ENTRY(entry_username));
    const char *password = gtk_entry_get_text(GTK_ENTRY(entry_password));

    if (authenticate(username, password)) {
        gtk_text_buffer_set_text(buffer, "Authentification réussie.\n", -1);
        // Afficher le menu dans la zone de texte
        printMenuToTextView();
    } else {
        gtk_text_buffer_set_text(buffer, "Échec de l'authentification.\n", -1);
    }

    return TRUE;
}






static gboolean on_send_choice_clicked(GtkWidget *widget, gpointer data) {
   
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_view));
    const char *choice_text = gtk_entry_get_text(GTK_ENTRY(entry_choice));
    int choice;
    sscanf(choice_text, "%d", &choice);

    write(client_socket, &choice, sizeof(int));




    switch (choice) {
        case 1: {
            char dateTime[100];
            read(client_socket, dateTime, sizeof(dateTime));
            gtk_text_buffer_insert_at_cursor(buffer,
                                             g_strdup_printf("\n Date et heure du serveur : %s\n", dateTime), -1);
            break;
        }
        case 2: {
            char fileList[1024];
            read(client_socket, fileList, sizeof(fileList));
            gtk_text_buffer_insert_at_cursor(buffer,
                                             g_strdup_printf("\n Liste des fichiers :\n%s\n", fileList), -1);
            break;
        }
        case 3: {
            const char *filename_text = gtk_entry_get_text(GTK_ENTRY(entry_filename));
            write(client_socket, filename_text, strlen(filename_text) + 1);

            char fileContent[1024];
            read(client_socket, fileContent, sizeof(fileContent));

            // Afficher le contenu du fichier dans la zone de texte de l'interface graphique
            gtk_text_buffer_insert_at_cursor(buffer,
                                             g_strdup_printf("Contenu du fichier :\n%s\n", fileContent), -1);
            break;
        }
        case 4: {
            char duration[50];
            read(client_socket, duration, sizeof(duration));
            gtk_text_buffer_insert_at_cursor(buffer,
                                             g_strdup_printf("\nDurée depuis la connexion : %s\n", duration), -1);
            break;
        }
        case 0: {
            exit(0);
        }
        default:
            break;
    }

    return TRUE;
}

void *run_gui(void *data) {
    int argc = 0;
    char **argv = NULL;

    gtk_init(&argc, &argv);

    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    // Set the title of the window
gtk_window_set_title(GTK_WINDOW(window), "Client GUI");

// Use CSS provider to style the window title bar
const gchar *window_css = "headerbar { background-color: #336699; color: #ffffff; }";
GtkCssProvider *provider_window = gtk_css_provider_new();
gtk_css_provider_load_from_data(provider_window, window_css, -1, NULL);
gtk_style_context_add_provider_for_screen(gdk_screen_get_default(), GTK_STYLE_PROVIDER(provider_window), GTK_STYLE_PROVIDER_PRIORITY_USER);

    gtk_container_set_border_width(GTK_CONTAINER(window), 10);
    gtk_widget_set_size_request(window, 400, 300);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_add(GTK_CONTAINER(window), box);

    entry_username = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(entry_username), "Nom d'utilisateur");
    gtk_box_pack_start(GTK_BOX(box), entry_username, FALSE, FALSE, 0);

    entry_password = gtk_entry_new();
    gtk_entry_set_visibility(GTK_ENTRY(entry_password), FALSE);
    gtk_entry_set_placeholder_text(GTK_ENTRY(entry_password), "Mot de passe");
    gtk_box_pack_start(GTK_BOX(box), entry_password, FALSE, FALSE, 0);

    text_view = gtk_text_view_new();
    gtk_text_view_set_editable(GTK_TEXT_VIEW(text_view), FALSE);
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(text_view), GTK_WRAP_WORD);
    gtk_box_pack_start(GTK_BOX(box), text_view, TRUE, TRUE, 0);

    // Ajout du bouton pour s'authentifier
// Ajout du bouton pour s'authentifier
GtkWidget *button = gtk_button_new_with_label("S'authentifier");

// Use GdkRGBA for color instead of GdkColor
GdkRGBA bg_color, border_color;
gdk_rgba_parse(&bg_color, "blue");
gdk_rgba_parse(&border_color, "darkblue");

// Use override functions to set background color and text color
gtk_widget_override_background_color(button, GTK_STATE_FLAG_NORMAL, &bg_color);
gtk_widget_override_color(button, GTK_STATE_FLAG_NORMAL, &bg_color);

// Use CSS to set border color
const gchar *button_css = "button { border: 2px solid darkblue; border-radius: 5px; }";
GtkCssProvider *provider = gtk_css_provider_new();
gtk_css_provider_load_from_data(provider, button_css, -1, NULL);
gtk_style_context_add_provider(gtk_widget_get_style_context(button), GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_USER);
g_signal_connect(button, "clicked", G_CALLBACK(on_button_clicked), NULL);
gtk_box_pack_start(GTK_BOX(box), button, FALSE, FALSE, 0);





    // Ajout de la zone de saisie pour le choix et le nom de fichier
    entry_choice = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(entry_choice), "Entrez votre choix");
    gtk_box_pack_start(GTK_BOX(box), entry_choice, FALSE, FALSE, 0);

    entry_filename = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(entry_filename), "Entrez le nom du fichier");
    gtk_box_pack_start(GTK_BOX(box), entry_filename, FALSE, FALSE, 0);

// Ajout du bouton pour envoyer le choix
GtkWidget *button_send_choice = gtk_button_new_with_label("Envoyer votre choix");

// Use GdkRGBA for color instead of GdkColor
GdkRGBA color_send_choice, border_color_send_choice;
gdk_rgba_parse(&color_send_choice, "blue");
gdk_rgba_parse(&border_color_send_choice, "darkblue");

// Use override functions to set background color and text color
gtk_widget_override_background_color(button_send_choice, GTK_STATE_FLAG_NORMAL, &color_send_choice);
gtk_widget_override_color(button_send_choice, GTK_STATE_FLAG_NORMAL, &color_send_choice);

// Use CSS to set border color
const gchar *button_send_choice_css = "button { border: 2px solid darkblue; border-radius: 5px; }";
GtkCssProvider *provider_send_choice = gtk_css_provider_new();
gtk_css_provider_load_from_data(provider_send_choice, button_send_choice_css, -1, NULL);
gtk_style_context_add_provider(gtk_widget_get_style_context(button_send_choice), GTK_STYLE_PROVIDER(provider_send_choice), GTK_STYLE_PROVIDER_PRIORITY_USER);

g_signal_connect(button_send_choice, "clicked", G_CALLBACK(on_send_choice_clicked), NULL);
gtk_box_pack_start(GTK_BOX(box), button_send_choice, FALSE, FALSE, 0);



    // Affichage de la fenêtre principale
    gtk_widget_show_all(window);

    gtk_main();

    return NULL;
}

int main() {
    pthread_t gui_thread;

    if (pthread_create(&gui_thread, NULL, run_gui, NULL) != 0) {
        fprintf(stderr, "Erreur lors de la création du thread de l'interface graphique.\n");
        exit(EXIT_FAILURE);
    }

    if ((client_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Erreur lors de la création du socket");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in server_address = {0};
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT);

    if (inet_pton(AF_INET, "127.0.0.1", &server_address.sin_addr) <= 0) {
        perror("Adresse invalide / Adresse non prise en charge");
        exit(EXIT_FAILURE);
    }

    if (connect(client_socket, (struct sockaddr *)&server_address, sizeof(server_address)) < 0) {
        perror("Échec de la connexion");
        exit(EXIT_FAILURE);
    }

    // Attendre que le thread de l'interface graphique soit prêt
    sleep(1);

    // Continuer les opérations réseau dans le thread principal
    int choice;
    int authenticated = 0;

    while (!authenticated) {
        authenticated = authenticate("abdoahmed", "abdoahmed");  // Vous pouvez adapter ces valeurs
        if (authenticated == 1) {
            do {
                // Attente de choix depuis l'interface graphique
                sleep(1); // Ajoutez une petite pause ici pour éviter une boucle rapide

            } while (choice != 0);
        }
    }

    // Attendre la fin du thread de l'interface graphique
    pthread_join(gui_thread, NULL);

    close(client_socket);
    return 0;
}

