from io import TextIOWrapper
from math import ceil
from typing import Sequence



def hashcode(string: str) -> int:
    """The original hashcode function to reverse. See https://docs.oracle.com/en/java/javase/21/docs/api/java.base/java/lang/String.html#hashCode()."""
    h = 0
    for character in string: h = 31 * h + ord(character)
    return ((h + 0x80000000) & 0xffffffff) - 0x80000000

def reverseHashcode(originalValue: int, lengthRange: int | Sequence[int, int | None] | None = None, maxOutputs: int | None = None, file: TextIOWrapper | None = None, printVerification: bool = False) -> None:
    """Given a hashcode `originalValue`, returns up to `maxOutputs` alphabetical strings (or an infinite number if not specified) that hash to `originalValue`.
    "Alphabetical string" here is a string of consecutive English uppercase or lowercase letters.
    
    Better tools already exist for this purpose (e.g. https://gist.github.com/pseudogravity/5a92ec51f5d97082358d06ef9b788e6b or https://github.com/hashcat/hashcat, neither of which are restricted to exclusively-alphabetical strings), but this is at least my own personal take on the reversal problem.
    
    `lengthRange`: the (inclusive) range of string lengths to search. If an integer is provided"""
    # `originalValue` should be in the range [-2^31, 2^31 - 1], or when shifted to unsigned integers, [0, 2^32 - 1].
    originalValue &= 0xffffffff
    stringLength = lengthRange if isinstance(lengthRange, int) else lengthRange[0] if isinstance(lengthRange, tuple) else 1
    # 31^stringLength, calculated incrementally during each iteration
    thirtyOnePower = 31**stringLength
    outputCount = 0
    # Until we reach the maximum number of outputs (if there is one):
    while (maxOutputs is None or outputCount < maxOutputs) and ((lengthRange[1] if isinstance(lengthRange, tuple) else lengthRange) is None or stringLength <= (lengthRange[1] if isinstance(lengthRange, tuple) else lengthRange)):
        # The strings are restricted to being A-Z/a-z only, which restricts each character's value to the range 65-90 for lowercase letters or 97-122 for uppercase letters. Therefore, for a k-character string, the total hash value of the string prior to the modulo can be at minimum 13/6*(31^k - 1) (for all 'A's) and at maximum 61/15*(31^k - 1) (for all 'z's).
        # At the same time, the pre-mod hash value must be `originalValue` plus some multiple of 2^32 (`offset`). Therefore, for an `offset` multiple to be valid, it must satisfy the equation 13/6*(31^k - 1) <= originalValue + 2^32*offset <= 61/15*(31^k - 1), or ceil(13/6*(31^k - 1)) <= offset <= floor(61/15*(31^k - 1)).
        for offset in range(ceil(((thirtyOnePower - 1)//6*13 - originalValue)/(1 << 32)), int(((thirtyOnePower - 1)//15*61 - originalValue)//(1 << 32)) + 1):
            # Then, as stated, the pre-mod value is of the form originalValue + 2^32*offset. Meanwhile, since each character must have at least a value of 65, exactly 13/6*(31^k - 1) of the pre-mod value is taken up by those combined portions, so we factor it out.
            premodHash = originalValue + offset*(1 << 32) - (thirtyOnePower - 1)//6*13
            # Similarly, since each lowercase letter has a value 32 greater than the corresponding uppercase letter, each possible combination of uppercase and lowercase letters for the string would come with a particular "deadweight" that then would be added to the pre-mod value. If all letters turn out to be 'A'/'a's, the letters' portion of the pre-mod value would be 0, so the maximum allowable deadweight for a case is premodHash itself; if instead all letters turn out to be 'Z'/'z's, the letters' portion of the pre-mod value would be 5/6*(31^k - 1), so the minimum allowable deadweight for a case is premodHash - 5/6*(31^k - 1).
            minDifference = premodHash - (thirtyOnePower - 1)//6*5
            maxDifference = premodHash
            # For easiness's sake later on we will represent the letter cases as binary numbers: the first bit will indicate if the first character is uppercase (0) or lowercase (1); the second bit will indicate the same for the second character; and so on. Under this representation, the maximum weight's combination--all lowercase letters--would be a string of k 1s (equating to the weight 16/15*(31^k - 1)), while the minimum weight's combination--all uppercase letters--would be a string of k 0s (obviously equating to 0, so we don't need to store that).
            minCombo = (1 << stringLength) - 1
            minComboValue = (thirtyOnePower - 1)//15*16
            maxCombo = 0
            # Copies 31^k so we don't lose the original
            thirtyOnePowerCopy = thirtyOnePower
            # Then, for each character in the string, we test if subtracting/adding 32*(31^(k-i)) from the current minimum/maximum value would still be greater/less than the minimum/maximum allowable difference. If so, we update min/maxDifference and update the relevant bit in min/maxCombo. 
            for i in range(stringLength):
                thirtyOnePowerCopy //= 31
                currentDifference = 32*thirtyOnePowerCopy
                if minComboValue - currentDifference >= minDifference:
                    minDifference += currentDifference
                    minCombo &= ~(1 << (stringLength - i - 1))
                if currentDifference <= maxDifference:
                    maxDifference -= currentDifference
                    maxCombo |= 1 << (stringLength - i - 1)
            
            # Then at this point we simply through all of the viable uppercase/lowercase combinations. (This is why we stored them as binary integers.)
            for letterCase in range(minCombo, maxCombo + 1):
                # For each one, we split the case's binary bits into a list, and then reverse the list since we'd always be iterating over it in backwards order. (The conversion to binary will remove leading zeroes from the representation, but we add these back later on, when we actually care about the list's length instead of just the 1s in the list like immediately below.)
                caseBin = [int(i) for i in bin(letterCase)[:1:-1]]
                # Then, calcuate the weight of the current letter case (which is simply a base-31 polynomial with the binary bits as coefficients, multipled by 32), and subtract that from a copy of premodHash.
                caseOffset = 0
                currentPower = 1
                for i in caseBin:
                    caseOffset += currentPower * i
                    currentPower *= 31
                premodHashCopy = premodHash - 32*caseOffset
                # At this point, all that is left is the weight of the letters themselves, which also form a base-31 polynomial with the letters' values (0-25) as coefficients.
                backwardsOutput = ''
                invalidFlag = False
                # Readd the leading zeroes (only to the end of the list since the list is in reverse order).
                for i in caseBin + [0]*(stringLength - len(caseBin)):
                    # Since every term in the aforementioned base-31 polynomial is, by definition, a multiple of 31 except for the last, we can incrementally determine each letter by moduloing the value by 31, picking the remainder out, and subtracting the remainder from premodHashCopy and dividing it by 31 (or equivalently dividing by 31 outright and flooring).
                    currentLetter = premodHashCopy % 31
                    # However, any coefficients outside the range 0-25 would correspond to non-alphabetic characters, which therefore invalidates the case. If that occurs, continue to the next case.
                    if currentLetter > 25:
                        invalidFlag = True
                        break
                    # Otherwise add the character to the current case's output string.
                    backwardsOutput += chr(65 + 32*i + currentLetter)
                    premodHashCopy //= 31
                if invalidFlag: continue
                # Then if the case wasn't invalidated, reverse the output string (since we determined the coefficients above from the last term to the first), and print it. Then increment outputCount and exit if appropriate.
                print(f"{backwardsOutput[::-1]}{' ( -> ' + str(hashcode(backwardsOutput[::-1])) + ')' if printVerification else ''}", file=file, flush=True)
                outputCount += 1
                if maxOutputs is not None and outputCount >= maxOutputs: return
        # Set up for the next offset
        stringLength += 1
        thirtyOnePower *= 31

reverseHashcode(int(input("Enter a hash number: ")), None)