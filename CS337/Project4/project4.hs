{--

This is a stub for project 4. You need to define the functions after their type signatures below.
You can make as many auxilary functions as you want, but it is important that the functions below
are defined with the given names.

Be sure to put comments in this file and include a readme file!

--}

--test databases

database0 = [("Lana Turner", "Buster Keaton", 80000, "Virginia Dare"), ("Ted Hughes", "Edna Millay", 70000, "Virginia Dare"), ("Virginia Dare", "Laurence Sterne", 100000, "Edna Millay"), ("Buster Keaton", "Lana Turner", 80000, "Ingrid Joyce"), ("James Joyce", "Ingrid Joyce", 60000, "Root"), ("Vanessa Redgrave", "Michael Redgrave", 110000, "James Joyce"), ("Michael Redgrave", "Vanessa Redgrave", 40000, "Vanessa Redgrave"), ("Edna Millay", "Ted Hughes", 70000, "Root"), ("Laurence Sterne", "Virginia Dare", 60000, "James Joyce"), ("Ingrid Joyce", "James Joyce", 60000, "Virginia Dare")]
database1 = []
database2 = [("Carol", "Eric", 200000, "Bob"), ("Fran", "Dan", 200000, "Eric"), ("Bob", "Alex", 100000, "Alex"), ("Dan", "Fran", 150000, "Carol"), ("Alex", "Bob", 100000, "Root"), ("Eric", "Carol", 300000, "Dan")]
database3 = [("Carol", "Eric", 200000, "Root"), ("Fran", "Dan", 200000, "Root"), ("Bob", "Alex", 100000, "Alex"), ("Dan", "Fran", 150000, "Root"), ("Alex", "Bob", 100000, "Root"), ("Eric", "Carol", 300000, "Root")]

-- type synonyms

type Employee = String
type Spouse = String
type Salary = Integer
type Manager = String
type Record = (Employee, Spouse, Salary, Manager)
type DB = [Record]
--define types for question No.6
type ManagerRecord = (String, Integer, Integer) --first element is the name of the manager, second element is the salary, third is the number of employee he/she manages
type ManagerList = [ManagerRecord]

--create function that retrieve different "fields" from a Record

getEmployee :: Record -> String
getEmployee (x,_,_,_) = x

getSpouse :: Record -> String
getSpouse (_,x,_,_) = x

getSalary :: Record ->Integer
getSalary (_,_,x,_) = x

getManager :: Record -> String
getManager (_,_,_,x) = x

--create function for getting "field" from a ManagerRecord
getName :: ManagerRecord -> String
getName (x,_,_) = x

getManagerSalary :: ManagerRecord -> Integer
getManagerSalary (_,x,_) = x

getNumberSubordinates :: ManagerRecord -> Integer
getNumberSubordinates (_,_,x) = x 

{--******************************************************start of helper functions******************************************************--}

--function that returns a Record given the name of an employee (search function), return empty Record if not found
searchRecord :: String -> DB -> Record
searchRecord name [] = ("","",0,"")		--name not found
searchRecord name (x:xs)			
    | (getEmployee x) == name = x		--name found
    | otherwise = searchRecord name xs 	--recursive case   

--helper function for grossly_overpaid that test if an employee's salary is higher than his/her supermanager(s)
overpowers :: Integer -> Record -> DB -> Bool
overpowers salary r db
    | (getManager r) == "Root" = salary > 100000 --base case (Root)
    | otherwise = (salary > getSalary(searchRecord (getManager r) db)) && (overpowers salary (searchRecord (getManager r) db) db) --recursive case

--helper function for spouse_manager_super and super_manager that test if an employee is being "supermanaged" by a given manager
beingManaged :: String -> Record -> DB -> Bool
beingManaged name r db
    | (getManager r) == "Root" = False --base case (Root)
    | otherwise = (name == (getManager r)) || (beingManaged name (searchRecord (getManager r) db) db) --recursive case

--helper function for super_manager that test if a given manager is a supermanager of both an employee and its spouse
manageBoth :: String -> DB -> DB-> Bool
manageBoth name [] db = False --base case
manageBoth name (x:xs) db = ((beingManaged name x db) && (beingManaged name (searchRecord (getSpouse x) db) db)) || (manageBoth  name xs db) --recursive case

--helper function for rich that finds the maximum amount of money that a pair of spouses earn
maxiumCoupleSalary :: DB -> DB -> Integer
maxiumCoupleSalary [] db = 0 --base case
maxiumCoupleSalary (x:xs) db = max ((getSalary x) + (getSalary (searchRecord (getSpouse x) db))) (maxiumCoupleSalary xs db) --recursive case

--function that swap the positions of a pair
swapPair :: (a, b) -> (b, a)
swapPair (a, b) = (b, a)

--helper function for No.6 that create a list of ManagerRecord (ManagerList with no duplicate) from a given database
createManagerList :: DB -> DB  -> ManagerList
createManagerList [] db = [] --base case
createManagerList (x:xs) db --recursive case
    | notRoot && (findIndex == -1) = (getEmployee manager, getSalary manager, 1): list--a new manager occur, update it to the ManagerList
    | notRoot = updateList findIndex list --the current employee's manager is already in the list, update the list
    | otherwise = list --the current employee's manager is Root, skip it
        where notRoot = (getManager x) /= "Root"
              findIndex = (findManager (getManager x) list)
              manager = searchRecord (getManager x) db --the manager's record
              list = (createManagerList xs db)


--helper function for No.6 that search a manager in a ManagerList and return the index (starts from 0, -1 if not found)
findManager :: String -> ManagerList -> Integer
findManager name xs = findManagerb name xs 0

--helper function for findManager
findManagerb :: String -> ManagerList -> Integer -> Integer
findManagerb name [] n = (-1) --manager not found in list
findManagerb name (x:xs) n
    | (getName x) == name = n --manager found
    | otherwise = findManagerb name xs (n + 1) --not in the current record, go to the next record

--helper function for No.6 that remove the nth record in a ManagerList and add a new updated ManagerRecord (updeate the number of subordinates) in the list
updateList :: Integer -> ManagerList -> ManagerList
updateList n list = xs ++ (z:ys)
    where xs = fst(splittedList) --split the ManagerList to perform the removal
          ys = tail(snd(splittedList)) --the record being removed is the first element of the second part of the list being splitted
          z = (getName old, getManagerSalary old, ((getNumberSubordinates old) + 1)) --this is the updated record (old is the record being removed)
          old =  head(snd(splitAt (fromInteger n) list))
          splittedList = splitAt (fromInteger n) list

--helper function that remove the nth record from a ManagerList
removeNthMList :: Integer -> ManagerList -> ManagerList
removeNthMList n list = (xs ++ (tail ys))
    where (xs, ys) = splitAt (fromInteger n) list

--helper function for No.6 that return the index of the record with the largest amount of salary in a ManagerList, return -1 if the ManagerList is empty
findRichest :: ManagerList -> Integer
findRichest list =  findRichestb list 0 (-1) 0

--helper function of findRichest
findRichestb :: ManagerList -> Integer -> Integer -> Integer -> Integer
findRichestb [] index maxIndex maxSalary = maxIndex --base case, end of list
findRichestb (x:xs) index maxIndex maxSalary
    | (getManagerSalary x) > maxSalary = findRichestb xs (index+1) index (getManagerSalary x) --the current manager has the largest salary, update the list
    | otherwise = findRichestb xs (index+1) maxIndex maxSalary --go to the next ManagerRecord

--helper function for No.6 that return the index of the record with the highest rank in a ManagerList, return -1 if the ManagerList is empty
findHighestRank :: ManagerList -> Integer
findHighestRank list =  findHighestRankb list 0 (-1) 0

--helper function of findHighestRank
findHighestRankb :: ManagerList -> Integer -> Integer -> Integer -> Integer
findHighestRankb [] index maxIndex maxRank = maxIndex --base case, end of list
findHighestRankb (x:xs) index maxIndex maxRank
    | (getNumberSubordinates x) > maxRank = findHighestRankb xs (index+1) index (getNumberSubordinates x) --the current manager has the highest rank, update the list
    | otherwise = findHighestRankb xs (index+1) maxIndex maxRank --go to the next ManagerRecord

--helper function for No.6 that return the index of the record with the highest worth in a ManagerList, return -1 if the ManagerList is empty
findWorthest :: ManagerList -> Integer
findWorthest list =  findWorthestb list 0 (-1) 0

--helper function of findWorthest
findWorthestb :: ManagerList -> Integer -> Integer -> Float -> Integer
findWorthestb [] index maxIndex maxWorth = maxIndex --base case, end of list
findWorthestb (x:xs) index maxIndex maxWorth
    | worthX > maxWorth = findWorthestb xs (index+1) index worthX --the current manager has the worthest, update the list
    | otherwise = findWorthestb xs (index+1) maxIndex maxWorth --go to the next ManagerRecord
        where worthX = (fromInteger (getManagerSalary x)) / (fromInteger (getNumberSubordinates x)) --calculate the worth

--helper function for No.7 that test if an employee if in a DB
inDB :: String -> DB -> Bool
inDB name [] = False		--name not found
inDB name (x:xs)			
    | (getEmployee x) == name = True		--name found
    | otherwise = inDB name xs 	--recursive case

{--******************************************************end of helper functions******************************************************--}

-- 1.

overpaid :: DB -> [String]
--use list comprehension
overpaid db = [(getEmployee x) | x <- db, if ((getManager x) == "Root") then ((getSalary x) > 100000) else ( (getSalary x) > getSalary(searchRecord (getManager x) db))]

grossly_overpaid :: DB -> [String]
--use list comprehension
grossly_overpaid db = [(getEmployee x) | x <- db, overpowers (getSalary x) x db]

-- 2.
spouse_manager :: DB -> [String]
--use list comprehension
spouse_manager db = [(getSpouse x) | x <- db, (getSpouse x) == (getManager x)]

spouse_manager_super :: DB -> [String]
--use list comprehension
spouse_manager_super db = [(getSpouse x) | x <- db, beingManaged (getSpouse x) x db]

-- 3.

super_manager :: DB -> [String]
--use list comprehension
super_manager db = [(getEmployee x) | x <- db, manageBoth (getEmployee x) db db]

-- 4.

nepotism :: DB -> [(String, String)]
--use list comprehension
nepotism db = [(getEmployee x,getEmployee y) | x <- db, y <-db, (getSpouse x == getManager y) && (getSpouse y == getManager x) && (getEmployee x /= getEmployee y)]

-- 5.

rich :: DB -> [(String, String)]
rich db = richb db db

--helper function for rich
richb :: DB -> DB -> [(String, String)]
richb [] db = []
richb (x:xs) db
    | ((maxiumCoupleSalary db db) == ((getSalary x) + (getSalary (searchRecord (getSpouse x) db)))) && not(elem (swapPair (getEmployee x, getSpouse x)) ys) = (getEmployee x, getSpouse x) : ys --one of the richest pairs found
    | otherwise = ys --the current couple is not the richest
        where
            ys = richb xs db --go to the next couple (recursive)

-- 6.

sorted_salaries	:: DB -> [String]
sorted_salaries db = sorted_salariesb (createManagerList db db)

--helper function for sorted_salaries
sorted_salariesb :: ManagerList -> [String]
sorted_salariesb [] = [] --base case
sorted_salariesb xs = (getName ((xs) !! (fromInteger richestIndex))) : sorted_salariesb (removeNthMList richestIndex xs) --recursive case
    where richestIndex = findRichest xs --find the index of the manager with the highest salary in the current ManagerList

sorted_rank :: DB -> [String]
sorted_rank db = sorted_rankb (createManagerList db db)

--helper function for sorted_rank
sorted_rankb :: ManagerList -> [String]
sorted_rankb [] = [] --base case
sorted_rankb xs = (getName ((xs) !! (fromInteger highestRankIndex))) : sorted_rankb (removeNthMList highestRankIndex xs) --recursive case
    where highestRankIndex = findHighestRank xs --find the index of the manager with the highest rank in the current ManagerList

sorted_worth :: DB -> [String]
sorted_worth db = sorted_worthb (createManagerList db db)

--helper function for sorted_worth
sorted_worthb :: ManagerList -> [String]
sorted_worthb [] = [] --base case
sorted_worthb xs = (getName ((xs) !! (fromInteger worthIndex))) : sorted_worthb (removeNthMList worthIndex xs) --recursive case
    where worthIndex = findWorthest xs --find the index of the worthest manager in the current ManagerList

-- 7.

normalize :: DB -> DB
normalize db = reverse (normalizeb db [] (toInteger (length db)) db)

--helper function for normalize
normalizeb :: DB -> DB -> Integer -> DB -> DB
normalizeb [] result len db
    | len == 0 = result --base case (the normalized database if fully formed)
    | otherwise = normalizeb [x | x <-db, not(elem x result)] result len db --special recursive case (using list comprehension) (not all Record has been added to the normalized database, run again)
normalizeb (x:xs) result len db
    | ((getManager x) == "Root") || (inDB (getManager x) result) = normalizeb xs (x:result) (len - 1) db --recursive case 1: a valid new record is found to be added to the normalized database
    | otherwise = normalizeb xs result len db --recursive case 2: current record can still not be added to the normalized database, go to the next record

