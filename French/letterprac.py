from subprocess import call
from random import random as rand
import math

numbers=[
    ["dix", "onze", "douze", "treize", "quatorze", "quinze", "seize", "dix-sept", "dix-huit", "dix-neuf"],
    ["vingt", "vingt et un", "vingt-deux", "vingt-trois", "vingt-quatre", "vingt-cinq", "vingt-six", "vingt-sept", "vingt-huit", "vingt-neuf"],
    ["trente", "trente et un", "trente-deux", "trente-trois", "trente-quartre", "trente-cinq", "trente-six", "trente-sept", "trente-huit", "trente-neuf"],
    ["quarante", "quarante et un", "quarante-deux", "quarante-trois", "quarante-quatre", "quarante-cinq", "quarante-six", "quarante-sept", "quarante-huit", "quarante-neuf"],
    ["cinquante", "cinquante et un", "cinquante-deux", "cinquante-trois", "cinquante-quatre", "cinquante-cinq", "cinquante-six", "cinquante-sept", "cinquante-huit", "cinquante-neuf"]
]

while True:
    index=int(math.floor(rand() * len(numbers)))
    index2=int(math.floor(rand() * len(numbers[index])))
    ins=["say", "-v", "Amelie", numbers[index][index2] + " ans"]
    call(ins)
    raw_input("answer")
    print(ins)
    raw_input("continue")
