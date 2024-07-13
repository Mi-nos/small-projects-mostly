import numpy as np

def parity7_4(bit_informacyjny, pozycje):
    return bit_informacyjny[pozycje[0]] ^ bit_informacyjny[pozycje[1]] ^ bit_informacyjny[pozycje[2]]

def Hamming_coding7_4(bit_informacyjny):
    #print("\nHamming 7,4, kodowanie")
    #print("informacja startowa", bit_informacyjny)

    bit_informacyjny.insert(1, 0)
    for i in range(2):
        bit_informacyjny.insert(0, 0)

    bit_informacyjny[0] = parity7_4(bit_informacyjny, (2, 4, 6))
    bit_informacyjny[1] = parity7_4(bit_informacyjny, (2, 5, 6))
    bit_informacyjny[3] = parity7_4(bit_informacyjny, (4, 5, 6))

    #print("informacja zakodowana", bit_informacyjny)
    return bit_informacyjny


def syndrom7_4(slowo_zakodowane, pozycje, pozycja_parity):
    return slowo_zakodowane[pozycja_parity] ^ parity7_4(slowo_zakodowane, pozycje)

def Hamming_decoding7_4(slowo_zakodowane):
    #print("\nHamming 7,4, dekodowanie")

    syndromy = []
    syndromy.append(syndrom7_4(slowo_zakodowane, (4, 5, 6), 3))
    syndromy.append(syndrom7_4(slowo_zakodowane, (2, 5, 6), 1))
    syndromy.append(syndrom7_4(slowo_zakodowane,(2, 4, 6), 0))

    #print("Oto wektor syndromów:", syndromy)
    if syndromy.count(1) == 0: #oznacza brak błędów w informacji
        #print("Brak błędów do poprawy, zwracam informację:")
        return [slowo_zakodowane[2], slowo_zakodowane[4], slowo_zakodowane[5], slowo_zakodowane[6]]
    elif syndromy.count(1) == 1: #oznacza jeden przestawiony bit, ale z bitów parzystości
        #ten blok nie ma wplywu na informację
        slowo_zakodowane[0] = parity7_4(slowo_zakodowane, (2, 4, 6))
        slowo_zakodowane[1] = parity7_4(slowo_zakodowane, (2, 5, 6))
        slowo_zakodowane[3] = parity7_4(slowo_zakodowane, (4, 5, 6))
        #print("Poprawiłem tylko bity parzystosci, oto informacja")
        #print("informacja odkodowana", [slowo_zakodowane[2], slowo_zakodowane[4], slowo_zakodowane[5], slowo_zakodowane[6]])
        return [slowo_zakodowane[2], slowo_zakodowane[4], slowo_zakodowane[5], slowo_zakodowane[6]]
    else: #oznacza jeden przestawiony bit, ale z bitów informacji
        #print("Poprawiam bit informacji: ")
        mult = 1
        pozycja_bita = 0
        for number in reversed(syndromy):
            pozycja_bita += number * mult
            mult *= 2
        #prosze zmien to int not pozniej
        #print("Bład jest na pozycji(indeksowanie od 1)", pozycja_bita)
        pozycja_bita = pozycja_bita - 1

        slowo_zakodowane[pozycja_bita] = int(not((slowo_zakodowane[pozycja_bita])))
        #print("informacja odkodowana", [slowo_zakodowane[2], slowo_zakodowane[4], slowo_zakodowane[5], slowo_zakodowane[6]])
        return [slowo_zakodowane[2], slowo_zakodowane[4], slowo_zakodowane[5], slowo_zakodowane[6]]
#insert
'''test = [1,1,0,1]
coded = Hamming_coding7_4(test)
coded[6] = 0

odkodowana_informacja = Hamming_decoding7_4(coded)'''

def bit_combinations(number_of_bits):
    max_num = np.power(2,number_of_bits)
    combinations = np.arange(max_num)
    t = [bin(n)[2:].zfill(number_of_bits) for n in combinations]
    t = [[int(bit) for bit in bit_group] for bit_group in t]
    t = np.vstack(t)
    return t

#bity informacji (jest ich 11)
#https://www.ece.unb.ca/tervo/ee4253/hamming2.shtml
def Hamming_coding15_11(bity_informacyjne):
    #n = 15, k = 11, m = 4
    #Stwórz macierz generującą G

    #print("\nHamming 15,11, kodowanie")
    #print("informacja startowa", bity_informacyjne)
    I = np.eye(11)

    pozycje = [2, 4, 5, 6, 8, 9, 10, 11, 12, 13, 14]

    P = bit_combinations(4)
    #print(P)
    P = P[1:, :][pozycje]
#    print(P)

    #print(P)
    a = P[:, 3]
    b = P[:, 2]
    c = P[:, 1]
    d = P[:, 0]

    #to jest poprawna macierz P
    P = np.vstack((a, b, c, d)).T
    #print(P)
    #do tutaj jest dobrze
    G = np.hstack((P, I))
    #print(G)
    c = np.dot(bity_informacyjne, G) % 2
    #teraz porządkowanie, bo nasze obecne słowo kodowane wygląda tak [p1, p2, p4, p8 | wiadomość]
    #print(c)

    kod = np.zeros_like(c)
    parities = c[:4]
    information = c[4:]

    kod[[0,1,3,7]] = parities
    kod[pozycje] = information

    #print("informacja zakodowana (pozycja 0, 1, 3, 7 to bity parzystości) ", kod)
    return c


def Hamming_decoding15_11(slowo_zakodowane):
    #print("\nHamming 15,11, dekodowanie")
    #musimy wyznaczyć macierz kontroli parzystości
    I = np.eye(4)

    pozycje = [2, 4, 5, 6, 8, 9, 10, 11, 12, 13, 14]

    P = bit_combinations(4)
    # print(P)
    P = P[1:, :][pozycje]
    #    print(P)

    # print(P)
    a = P[:, 3]
    b = P[:, 2]
    c = P[:, 1]
    d = P[:, 0]

    # to jest poprawna macierz P
    P = np.vstack((a, b, c, d)).T

    H = np.hstack((I, P.T))

    #tablica syndromów działa w przypadku ułożenia bitów tak jak jest c w encoding
    syndromy = np.dot(slowo_zakodowane, H.T) % 2 #to jest nasz wektor syndromów

    #print("Oto wektor syndromów:", syndromy)

    if np.count_nonzero(syndromy) <= 1: #żaden bit informacji nie został przestawiony
        #print("Brak błędów do poprawy, zwracam informację:")
        #print("informacja odkodowana", slowo_zakodowane[4:])
        return slowo_zakodowane[4:]

    else:  # detekcja przestawionego bitu informacyjnego
        #print("Poprawiam bit informacji")

        multipliers = [1, 2, 4, 8]
        result = np.sum(np.multiply(syndromy, multipliers))
        result = int(result) - 1

        #czemu tutaj jest castowanie? bo iterujemy po obiekcie syndromy, to obiekt numpy który przechowuje floaty
        #print("pozycja bita do naprawy (indeksowanie od 0)", result)

        kod = np.zeros_like(slowo_zakodowane)
        parities = slowo_zakodowane[:4]
        information = slowo_zakodowane[4:]

        kod[[0, 1, 3, 7]] = parities
        kod[pozycje] = information

        kod[result] = kod[result] ^ 1
        #print("informacja odkodowana", kod[pozycje])
        return kod[pozycje]


'''bit_info = np.array([0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0])
c = Hamming_coding15_11(bit_info)
#szum = np.array([0, 0, 0, 0, 1, 0, 1, 0, 1, 1, 0, 1, 1, 0, 0])
#               [p, p, D, p, D, D, D, p, D, D, D, D, D, D, D] tak jest w oryginalnym Hammingu

#               [p, p, p, p, D, D, D, D, D, D, D, D, D, D, D] taki ciąg daje nam wektor c z Hamming_coding15_11
szum = np.array([0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0])


print(c)
decoded = Hamming_decoding15_11(szum)

print(bit_info == decoded)'''

'''Na czym polegał problem? 
[p, p, p, p, D, D, D, D, D, D, D, D, D, D, D] taki ciąg daje nam wektor c z Hamming_coding15_11

po stworzeniu macierzy syndromów w Hamming decoding należy przestawić macierz powyższą na poniższą

[p, p, D, p, D, D, D, p, D, D, D, D, D, D, D] tak jest w oryginalnym Hammingu
'''