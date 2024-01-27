
---

# Baza Danych FranekQL

FranekQL to baza danych zaimplementowana w języku C++, oferująca prosty, lecz potężny język zapytań do manipulacji danymi. Obsługuje podstawowe operacje CRUD (Create, Read, Update, Delete) na tabelach i wierszach, a także zaawansowane funkcje, takie jak klucze obce, relacje i typy danych.

## Cechy i Ograniczenia

### Typy Danych
FranekQL obsługuje podstawowe typy danych, w tym:
- `INTEGER` (32-bitowa liczba całkowita ze znakiem, np. `1`)
- `TEXT` (łańcuch znaków w pojedynczych cudzysłowach, np. `'Hello World!'`)
- `BOOLEAN` (prawda/fałsz, np. `true`)
- `FLOAT` (liczba zmiennoprzecinkowa, np. `1.0`)
- `DOUBLE` (liczba zmiennoprzecinkowa o podwójnej precyzji, np. `1.0`)
- `CHAR` (znak, tylko jeden i w pojedynczych cudzysłowach, np. `'a'`)
- `DATE` (format: `%Y-%m-%d`, np. `2021-01-01`)
- `TIME` (format: `%H:%M:%S`, np. `12:00:00`)
- `DATETIME` (format: `%Y-%m-%dT%H:%M:%S`, np. `2021-01-01T12:00:00`)

Każda kolumna w tabeli musi mieć określony typ danych, a wszystkie dane w tej kolumnie muszą być tego samego typu. \
Tabele mogą mieć dowolną liczbę kolumn, ale każda tabela musi posiadać klucz główny. \
Klucze obce mogą być dodawane do tabel, ale muszą odnosić się do klucza głównego w innej tabeli lub do kolumny z ograniczeniem unikalności.

### Operacje na Tabelach
FranekQL obsługuje następujące operacje:

- **CREATE TABLE**: Tworzy nową tabelę. Na przykład:
  ```markdown
  CREATE TABLE studenci ( 
    id INTEGER PRIMARY_KEY, 
    nazwa STRING NOT_NULL,
    FOREIGN_KEY id REFERENCES inna_nazwa_tabeli inna_nazwa_kolumny
  );

  CREATE TABLE nazwa_tabeli (
    nazwa_kolumny typ_danych,
    ograniczenie1 ograniczenie2, 
    FOREIGN_KEY nazwa_kolumny REFERENCES inna_nazwa_tabeli inna_nazwa_kolumny, ...
  );
  ```
  Uwaga: Musi być określony klucz główny jako `PRIMARY_KEY`, a nie `PRIMARY KEY`. \
  Ograniczenie `NOT_NULL` jest określone jako `NOT_NULL`, a nie `NOT NULL`. \
  Nie można odwoływać się do klucza obcego, jeśli jeszcze nie istnieje. \
  Aby dodać klucz obcy, użyj `FOREIGN_KEY`. \
  Aby dodać więcej niż jedno ograniczenie, po prostu wymień je oddzielone spacjami. \
  Nazwy tabel muszą być unikalne w całej bazie danych. \
  Nazwy kolumn muszą być unikalne w obrębie tabeli. \
  Klucze obce można dodawać do tabeli tylko wtedy, gdy odnoszą się do kolumny z kluczem głównym lub z ograniczeniem unikalności.

- **DROP TABLE**: Usuwa istniejącą tabelę. Na przykład:
  ```markdown
  DROP TABLE studenci;
  
  DROP TABLE nazwa_tabeli;
  ```
- **ALTER TABLE**: Modyfikuje istniejącą tabelę, dodając lub usuwając kolumny. Na przykład:
  ```markdown
  ALTER TABLE studenci ADD COLUMN wiek INTEGER;
  ALTER TABLE studenci DROP COLUMN wiek;
  ALTER TABLE studenci ADD COLUMN wiek INTEGER NOT_NULL ADD COLUMN ocena INTEGER DROP COLUMN ocena;

  ALTER TABLE nazwa_tabeli 
  ADD COLUMN nazwa_kolumny typ_danych ograniczenie1 ograniczenie2 
  ADD COLUMN nazwa_kolumny typ_danych ograniczenie1 ograniczenie2 
  DROP COLUMN nazwa_kolumny
  DROP COLUMN nazwa_kolumny2
  FOREIGN_KEY nazwa_kolumny REFERENCES nazwa_tabeli nazwa_kolumny;
  FOREIGN_KEY nazwa_kolumny2 REFERENCES nazwa_tabeli2 nazwa_kolumny2;
  ```

### Operacje na Wierszach
FranekQL obsługuje następujące operacje na wierszach:

- **INSERT INTO**: Dodaje nowy wiersz do tabeli. Na przykład:
  ```markdown
  INSERT INTO studenci (ID, NAZWA) VALUES (1, 'John');

  INSERT INTO nazwa_tabeli (nazwa_kolumny1, nazwa_kolumny2) VALUES (wartość1, wartość2);
  ```
  Uwaga: Składnia insert musi być dokładnie taka, jak pokazano powyżej. \
  Baza danych używa także wartośći typu NULL, aby wstawić wartość NULL do kolumny trzeba użyć NULL zamiast wartości 
  (NULL nie 'NULL' ani 'null').


- **SELECT**: Pobiera wiersze z tabeli. Na przykład:
  ```markdown
  SELECT id, nazwa FROM studenci WHERE wiek > 18;
  
  SELECT nazwa_kolumny1, nazwa_kolumny2 
  FROM nazwa_tabeli 
  WHERE nazwa_kolumny1 = wartość1 AND nazwa_kolumny2 <> wartość2;
  ```
  Uwaga: Instrukcja `SELECT` może być używana z nawiasami, `OR`, `AND`, `=`, `<>`, `<=>` oraz wszystkimi operacjami porównania między wartościami. Można także używać `COLUMN_NAME IS_NULL` lub `IS_NOT_NULL`. Możliwe jest użycie dowolnej liczby nawiasów i grup warunków. Na przykład, `((((COLUMN_NAME IS NULL) AND COLUMN_NAME > 5) OR COLUMN_NAME < 1))` jest poprawną składnią.

## Jak Korzystać

Aby używać FranekQL należy wpisywać zapytania w konsoli oraz zakończyć je średnikiem. \
Parser stworzony w ramach projektu nie jest wrażliwy na znaki białe, więc można używać ich w dowolnych miejscach. \
Należy jednak pamiętać, że jest wrażliwy na wielkość liter, więc należy pisać wszystkie słowa kluczowe wielkimi literami.

## Zapisywanie i Ładowanie Stanu Bazy Danych

FranekQL obsługuje zapisywanie i ładowanie stanu bazy danych do pliku:
- Polecenie `\s` zapisuje wszystkie poprawnie wykonane zapytania do pliku o nazwie `event_source_backup.franekql`.
- Polecenie `\d <scieżka_do_pliku>` ładuje zapytania z określonego pliku i wykonuje je w kolejności.
- Polecenie `\h` wyświetla historię ostatnich 5 zapytań.
- Polecenie `\q` kończy program i zapisuje jeszcze niezapisane zapytania do pliku o nazwie `event_source_backup.franekql`.

## UWAGA
Polecenie \d także będzie zapisywać zapytania do pliku `event_source_backup.franekql`!  \
W razie gdyby plik o tej nazwie już istniał będzie do niego dopisywać zapytania.  \
Może to spodować błędną kopię zapsową bazy danych. \
Proszę unikać używania pliku `event_source_backup.franekql`przy użyciu polecenia `\d`




## PRZYKŁADOWY SKRYPT (Niektóre rzeczy specjalnie dają ERROR aby pokazać, że baza danych działa prawidłowo)


CREATE TABLE Orders (
    OrderID INTEGER PRIMARY_KEY,
    CustomerID INTEGER UNIQUE,
    ProductID INTEGER NOT_NULL,
    Quantity INTEGER NOT_NULL,
    CreationDate DATE NOT_NULL,
    CreationDateTime DATETIME,
    CreationTime TIME,
    FunnyCharacter CHAR
);

ALTER TABLE Orders ADD COLUMN OrderNullColumn INTEGER;


INSERT INTO Orders (OrderID, CustomerID, ProductID, Quantity, CreationDate, CreationDateTime, CreationTime, FunnyCharacter, OrderNullColumn)
VALUES (1, 1, 1, 10, '2022-01-01', '2022-01-01T10:00:00', '10:00:00', 'A', NULL);

INSERT INTO Orders (OrderID, CustomerID, ProductID, Quantity, CreationDate, CreationDateTime, CreationTime, FunnyCharacter, OrderNullColumn)
VALUES (2, 2, 2, 20, '2022-01-02', '2022-01-02T11:00:00', '11:00:00', 'B', NULL);

INSERT INTO Orders (OrderID, CustomerID, ProductID, Quantity, CreationDate, CreationDateTime, CreationTime, FunnyCharacter, OrderNullColumn)
VALUES (3, 3, 3, 30, '2022-01-03', '2022-01-03T12:00:00', '12:00:00', 'C', NULL);

INSERT INTO Orders (OrderID, CustomerID, ProductID, Quantity, CreationDate, CreationDateTime, CreationTime, FunnyCharacter, OrderNullColumn)
VALUES (5, 5, 5, 31, '2022-01-03', '2022-01-03T12:00:00', '12:00:00', 'CASF', NULL);

SELECT * FROM Orders WHERE Quantity > 5 AND (Quantity > 50 OR OrderNullColumn IS_NULL);


SELECT * FROM Orders WHERE Quantity > 12;

ALTER TABLE Orders DROP COLUMN CreationDate;

SELECT * FROM Orders WHERE Quantity > 12;


SELECT * FROM Orders WHERE Quantity > 12;

SELECT * FROM Orders WHERE Quantity < 15;


SELECT * FROM Orders WHERE OrderID IS_NOT_NULL AND ProductID = 1;

SELECT * FROM Orders WHERE (CustomerID = 2 OR Quantity < 30);


DROP TABLE;

SELECT * FROM Orders;


---
