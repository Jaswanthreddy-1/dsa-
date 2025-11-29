#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MOVIES_MAX 10
#define SHOWS_MAX 5
#define SEATS_MAX 30
#define USERS_MAX 100
#define MOVIE_NAME_MAX 100
#define SHOW_TIME_MAX 30
#define ADMIN_PASS "admin123" // Change password as needed

typedef struct {
    int seatStatus[SEATS_MAX]; // 0: available, 1: booked
} ShowSeats;

typedef struct {
    char time[SHOW_TIME_MAX];
    ShowSeats seats;
} ShowTime;

typedef struct {
    char name[MOVIE_NAME_MAX];
    int id;
    ShowTime shows[SHOWS_MAX];
    int numShows;
} Movie;

typedef struct {
    int movieId;
    int showIndex;
    int seatNo;
    char userName[50];
} Booking;

Movie movies[MOVIES_MAX];
int movieCount = 0;
Booking userBookings[USERS_MAX];
int bookingCount = 0;

void banner();
void defaultMovies();
void clearInput();
void customerMenu();
void adminMenu();
int getInt();
void listMovies();
void listShows(Movie* m);
void displaySeats(Movie* m, int showIdx);
void bookTicket();
void cancelTicket();
void viewMyBookings();
void adminLogin();
void adminAddMovie();
void adminEditMovie();
void adminDeleteMovie();
void adminEditShowTimes();
void adminResetSeats();
int findMovieById(int id);
void saveBooking(int movieId, int showIndex, int seatNo, const char* userName);

// ------- Utility Functions --------

void banner() {
    printf("\n============================================\n");
    printf("     MOVIE TICKET BOOKING SYSTEM (DSA)\n");
    printf("============================================\n");
}

void clearInput() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

int getInt() {
    int value, ret;
    ret = scanf("%d", &value);
    if (ret!=1) {
        clearInput();
        return -1;
    }
    return value;
}

void defaultMovies() {
    movieCount = 3;
    strcpy(movies[0].name, "Avengers: Endgame");
    movies[0].id = 101;
    movies[0].numShows = 3;
    strcpy(movies[0].shows[0].time, "10:00 AM");
    strcpy(movies[0].shows[1].time, "2:00 PM");
    strcpy(movies[0].shows[2].time, "7:00 PM");

    strcpy(movies[1].name, "Inception");
    movies[1].id = 102;
    movies[1].numShows = 2;
    strcpy(movies[1].shows[0].time, "1:00 PM");
    strcpy(movies[1].shows[1].time, "6:00 PM");

    strcpy(movies[2].name, "Interstellar");
    movies[2].id = 103;
    movies[2].numShows = 3;
    strcpy(movies[2].shows[0].time, "10:30 AM");
    strcpy(movies[2].shows[1].time, "5:00 PM");
    strcpy(movies[2].shows[2].time, "9:00 PM");

    // Reset seats for all shows/movies
    for(int m=0; m < movieCount; m++)
        for(int s=0; s < movies[m].numShows; s++)
            for(int ss=0; ss<SEATS_MAX; ss++)
                movies[m].shows[s].seats.seatStatus[ss]=0;
}

// ---------- CUSTOMER MODULE ----------

void listMovies() {
    printf("\n----- Movies -----\n");
    for(int i=0;i<movieCount;i++) {
        printf("[%d] %s\n", movies[i].id, movies[i].name);
    }
}

void listShows(Movie* m) {
    printf("Shows for %s:\n", m->name);
    for(int i=0; i < m->numShows; i++) {
        printf("%d) %s\n", i+1, m->shows[i].time);
    }
}

void displaySeats(Movie* m, int showIdx) {
    printf("Seats for %s at %s (0=Available, 1=Booked):\n", m->name, m->shows[showIdx].time);
    for(int i=0;i<SEATS_MAX;i++) {
        printf("Seat %02d: %d\t", i+1, m->shows[showIdx].seats.seatStatus[i]);
        if((i+1)%10==0) printf("\n");
    }
}

void saveBooking(int movieId, int showIndex, int seatNo, const char* userName) {
    if(bookingCount<USERS_MAX) {
        userBookings[bookingCount].movieId = movieId;
        userBookings[bookingCount].showIndex = showIndex;
        userBookings[bookingCount].seatNo = seatNo;
        strncpy(userBookings[bookingCount].userName, userName, sizeof(userBookings[bookingCount].userName)-1);
        bookingCount++;
    }
}

void bookTicket() {
    banner();
    char user[50];
    printf("Enter your name: ");
    clearInput(); fgets(user, 50, stdin);
    user[strcspn(user,"\n")] = 0;

    listMovies();
    printf("Enter Movie ID to book: ");
    int id = getInt();
    int mIdx = findMovieById(id);
    if(mIdx == -1) { printf("Invalid Movie ID\n"); return; }

    listShows(&movies[mIdx]);
    printf("Enter show number: ");
    int sIdx = getInt()-1;
    if(sIdx<0 || sIdx>=movies[mIdx].numShows) { printf("Invalid show number\n"); return; }

    displaySeats(&movies[mIdx], sIdx);
    printf("Enter Seat Number to Book (1-%d): ", SEATS_MAX);
    int seatNum = getInt();
    if(seatNum<1 || seatNum>SEATS_MAX) { printf("Invalid seat!\n"); return; }
    seatNum--;

    if (movies[mIdx].shows[sIdx].seats.seatStatus[seatNum]==0) {
        movies[mIdx].shows[sIdx].seats.seatStatus[seatNum]=1;
        saveBooking(movies[mIdx].id, sIdx, seatNum+1, user);
        printf("Booking Confirmed! %s - %s - Seat %d\n", movies[mIdx].name, movies[mIdx].shows[sIdx].time, seatNum+1);
    } else {
        printf("Seat is already booked!\n");
    }
}

void cancelTicket() {
    banner();
    char user[50];
    printf("Enter your name: ");
    clearInput(); fgets(user, 50, stdin);
    user[strcspn(user,"\n")] = 0;
    listMovies();
    printf("Enter Movie ID for cancellation: ");
    int id = getInt();
    int mIdx = findMovieById(id);
    if(mIdx==-1) { printf("Invalid Movie ID\n"); return; }
    listShows(&movies[mIdx]);
    printf("Enter Show number: ");
    int sIdx = getInt()-1;
    if(sIdx<0 || sIdx>=movies[mIdx].numShows) { printf("Invalid show number\n"); return; }
    displaySeats(&movies[mIdx], sIdx);

    printf("Enter Seat Number to Cancel (1-%d): ", SEATS_MAX);
    int seatNum = getInt();
    if(seatNum<1 || seatNum>SEATS_MAX) { printf("Invalid seat!\n"); return; }
    seatNum--;

    int found = 0;
    for(int i=0; i<bookingCount; i++) {
        if(userBookings[i].movieId == movies[mIdx].id &&
           userBookings[i].showIndex == sIdx &&
           userBookings[i].seatNo == seatNum+1 &&
           strcmp(userBookings[i].userName, user)==0) {
            found = 1;
            movies[mIdx].shows[sIdx].seats.seatStatus[seatNum]=0;
            printf("Booking cancelled for %s, %s, Seat %d\n",
               movies[mIdx].name, movies[mIdx].shows[sIdx].time, seatNum+1);
            // Remove booking from userBookings
            for(int j=i; j < bookingCount-1; j++) userBookings[j] = userBookings[j+1];
            bookingCount--;
            break;
        }
    }
    if(!found) printf("No booking found for given details.\n");
}

void viewMyBookings() {
    banner();
    char user[50];
    printf("Enter your name to view bookings: ");
    clearInput(); fgets(user, 50, stdin);
    user[strcspn(user,"\n")] = 0;
    int found = 0;
    printf("\nYour Bookings:\n");
    for(int i=0;i<bookingCount;i++) {
        if(strcmp(userBookings[i].userName, user)==0) {
            int mIdx = findMovieById(userBookings[i].movieId);
            int showIdx = userBookings[i].showIndex;
            printf("Movie: %s\n", movies[mIdx].name);
            printf("Show: %s\n", movies[mIdx].shows[showIdx].time);
            printf("Seat: %d\n\n", userBookings[i].seatNo);
            found = 1;
        }
    }
    if(!found) printf("No bookings found for user %s.\n", user);
}

// ---------- ADMIN MODULE ----------

void adminLogin() {
    char adminPassword[50];
    printf("Enter admin password: ");
    clearInput(); fgets(adminPassword, 50, stdin);
    adminPassword[strcspn(adminPassword,"\n")] = 0;
    if(strcmp(adminPassword,ADMIN_PASS)==0) {
        adminMenu();
    } else {
        printf("Incorrect Password!\n");
    }
}

void adminMenu() {
    int choice;
    while(1) {
        banner();
        printf("---- Admin Panel ----\n");
        printf("1. Add Movie\n");
        printf("2. Edit Movie Name\n");
        printf("3. Delete Movie\n");
        printf("4. Edit Show Times\n");
        printf("5. Reset Seats for Movie Show\n");
        printf("6. Back to Main Menu\n");
        printf("Enter choice: ");
        choice = getInt();
        switch(choice) {
            case 1: adminAddMovie(); break;
            case 2: adminEditMovie(); break;
            case 3: adminDeleteMovie(); break;
            case 4: adminEditShowTimes(); break;
            case 5: adminResetSeats(); break;
            case 6: return;
            default: printf("Invalid choice!\n");
        }
    }
}

void adminAddMovie() {
    if(movieCount >= MOVIES_MAX) {
        printf("Maximum movies reached! Delete a movie first.\n");
        return;
    }
    Movie newMovie;
    printf("Enter Movie name: ");
    clearInput(); fgets(newMovie.name, MOVIE_NAME_MAX, stdin);
    newMovie.name[strcspn(newMovie.name,"\n")] = 0;
    printf("Enter Movie ID (unique): ");
    int id=getInt();
    if(findMovieById(id)!=-1) { printf("ID already exists!\n"); return; }
    newMovie.id=id;
    printf("Enter number of shows (max %d): ", SHOWS_MAX);
    int ns=getInt();
    if(ns<1 || ns>SHOWS_MAX) { printf("Invalid number!\n"); return; }
    newMovie.numShows=ns;
    for(int i=0;i<ns;i++) {
        printf("Enter show time %d: ", i+1);
        clearInput(); fgets(newMovie.shows[i].time, SHOW_TIME_MAX, stdin);
        newMovie.shows[i].time[strcspn(newMovie.shows[i].time,"\n")] = 0;
        for(int j=0;j<SEATS_MAX;j++) newMovie.shows[i].seats.seatStatus[j]=0;
    }
    movies[movieCount++]=newMovie;
    printf("Movie added successfully.\n");
}

void adminEditMovie() {
    listMovies();
    printf("Enter Movie ID to edit: ");
    int id=getInt();
    int idx=findMovieById(id);
    if(idx==-1) { printf("Movie not found!\n"); return; }
    printf("Current name: %s\n", movies[idx].name);
    printf("Enter new name: ");
    clearInput(); fgets(movies[idx].name, MOVIE_NAME_MAX, stdin);
    movies[idx].name[strcspn(movies[idx].name,"\n")] = 0;
    printf("Movie name updated.\n");
}

void adminDeleteMovie() {
    listMovies();
    printf("Enter Movie ID to delete: ");
    int id=getInt();
    int idx=findMovieById(id);
    if(idx==-1) { printf("Movie not found!\n"); return; }
    for(int i=idx; i<movieCount-1; i++) movies[i]=movies[i+1];
    movieCount--;
    printf("Movie deleted.\n");
}

void adminEditShowTimes() {
    listMovies();
    printf("Enter Movie ID to edit show times: ");
    int id=getInt();
    int idx=findMovieById(id);
    if(idx==-1) { printf("Movie not found!\n"); return; }
    printf("Movie: %s\n", movies[idx].name);
    for(int i=0;i<movies[idx].numShows;i++) {
        printf("Current time for show %d: %s\n", i+1, movies[idx].shows[i].time);
        printf("Enter new time for show %d (or press ENTER to keep unchanged): ", i+1);
        clearInput(); char temp[SHOW_TIME_MAX];
        fgets(temp, SHOW_TIME_MAX, stdin);
        if(temp[0]!='\n') {
            temp[strcspn(temp,"\n")] = 0;
            strcpy(movies[idx].shows[i].time, temp);
        }
    }
    printf("Show times updated.\n");
}

void adminResetSeats() {
    listMovies();
    printf("Enter Movie ID to reset seats: ");
    int id = getInt();
    int idx=findMovieById(id);
    if(idx==-1) { printf("Movie not found!\n"); return; }
    listShows(&movies[idx]);
    printf("Enter Show number: ");
    int sIdx=getInt()-1;
    if(sIdx<0 || sIdx>=movies[idx].numShows) { printf("Invalid show number!\n"); return; }
    for(int j=0;j<SEATS_MAX;j++) movies[idx].shows[sIdx].seats.seatStatus[j]=0;
    printf("All seats reset and available.\n");
}

// ------- Movie Search ---------
int findMovieById(int id) {
    for(int i=0;i<movieCount;i++) if(movies[i].id==id) return i;
    return -1;
}

// -------- Main Menu ----------
void customerMenu() {
    int ch;
    while(1) {
        banner();
        printf("1. View Movies\n");
        printf("2. Book Ticket\n");
        printf("3. Cancel Ticket\n");
        printf("4. View My Bookings\n");
        printf("5. Admin (Restricted)\n");
        printf("6. Exit\n");
        printf("Enter choice: ");
        ch=getInt();
        switch(ch) {
            case 1: listMovies(); break;
            case 2: bookTicket(); break;
            case 3: cancelTicket(); break;
            case 4: viewMyBookings(); break;
            case 5: adminLogin(); break;
            case 6: printf("Exiting...\n"); return;
            default: printf("Invalid choice!\n");
        }
        printf("\nPress ENTER to continue...\n"); clearInput();
    }
}

// ----------------- MAIN -------------------

int main() {
    defaultMovies();
    customerMenu();
    return 0;
}
