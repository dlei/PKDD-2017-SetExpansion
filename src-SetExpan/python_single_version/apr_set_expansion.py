import sys
import numpy as np
from statistics import mean, pstdev
import math

def getSeedEids():
  # apr seeds options:
  return [114186, 53862]
  #return [53862, 114149]
  #return [114149, 99092]
  #return [99092, 93219]
  #return [93219, 53863]
  #return [53863, 24981]
  #return [24981, 53914]
  #return [53914, 42708]


  # wiki seeds
  #return [290971, 56418] #states
  #return [448861, 60392] #continent
  #return [644125, 114450] #country
  #return [284961, 367649, 560528] #china provinces
  #return [458960, 500066, 34818] #canadian provinces
  #return [123333, 358790, 333470] #Mexico states
  #return [36363, 94898] #political parties
  #return [49975, 17740] #presidential candidates
  #return [33336, 56227]

def getParent():
  return -1
  #return 36363
  #return 644125
  #return 170137
  #return 644125

def getUpperLevel():
  return []
  #return [36363, 94898]
  #return [60392, 206066, 29190, 448861]
  #return [644125, 114450, 112564, 640449, 408545, 297492]
  #return [114450, 644125, 132050, 292563, 222703, 309528, 408545, 170137]

def loadEidToEntityMap(filename):
    map = {}
    with open(filename, 'r') as fin:
        for line in fin:
            seg = line.strip('\r\n').split('\t')
            map[int(seg[1])] = seg[0]
    return map

def loadFeaturesAndEidMap(filename):
    featuresetByEid = {}
    eidsByFeature = {}
    with open(filename, 'r') as fin:
        for line in fin:
            seg = line.strip('\r\n').split('\t')
            eid = int(seg[0])
            feature = seg[1]
            if eid not in featuresetByEid:
                featuresetByEid[eid] = set([feature])
            else:
                featureset = featuresetByEid[eid]
                featureset.add(feature)
                featuresetByEid[eid] = featureset
            if feature not in eidsByFeature:
                eidsByFeature[feature] = set([eid])
            else:
                eidSet = eidsByFeature[feature]
                eidSet.add(eid)
                eidsByFeature[feature] = eidSet
    return featuresetByEid, eidsByFeature

def loadWeightByEidAndFeatureMap(filename):
  weightByEidAndFeatureMap = {}
  with open(filename, 'r') as fin:
    for line in fin:
      seg = line.strip('\r\n').split('\t')
      eid = int(seg[0])
      feature = seg[1]
      weight = float(seg[3])
      weightByEidAndFeatureMap[(eid, feature)] = weight
  return weightByEidAndFeatureMap

def getCombinedWeightByFeatureMapAndNumOfMatchesByFeatureMap(seedEids, featuresByEidMap, weightByEidAndFeatureMap):
  combinedWeightByFeatureMap = {}
  numOfMatchesByFeatureMap = {}
  for seed in seedEids:
    featuresOfSeed = featuresByEidMap[seed]
    for sg in featuresOfSeed:
      if sg in combinedWeightByFeatureMap:
        combinedWeightByFeatureMap[sg] += weightByEidAndFeatureMap[(seed, sg)]
        numOfMatchesByFeatureMap[sg] += 1.0
      else:
        combinedWeightByFeatureMap[sg] = weightByEidAndFeatureMap[(seed, sg)]
        numOfMatchesByFeatureMap[sg] = 1.0

  #for e in combinedWeightByFeatureMap:
  #  combinedWeightByFeatureMap[e] = numOfMatchesByFeatureMap[sg]*numOfMatchesByFeatureMap[sg]/combinedWeightByFeatureMap[e]
  return combinedWeightByFeatureMap, numOfMatchesByFeatureMap

def getFeatureSim(eid, seed, weightByEidAndFeatureMap, features, dpMap=None):
  '''
  sim = 0.0
  norm_eid = 0.0
  norm_seed = 0.0
  for f in features:
      if (eid, f) in weightByEidAndFeatureMap:
          weight_eid = weightByEidAndFeatureMap[(eid, f)]
      else:
          weight_eid = 0.0
      if (seed, f) in weightByEidAndFeatureMap:
          weight_seed = weightByEidAndFeatureMap[(seed, f)]
      else:
          weight_seed = 0.0
      sim += 1.0 * weight_eid * weight_seed
      norm_eid += 1.0 * weight_eid * weight_eid
      norm_seed += 1.0 * weight_seed * weight_seed
  divider = math.sqrt(norm_eid)*math.sqrt(norm_seed)
  if divider > 0:
      return sim/divider
  else:
      return 0.0
  '''
  # jaccard sim
  key = frozenset([eid, seed])
  if dpMap is not None:
    if key in dpMap:
      return dpMap[key], dpMap
  simWithSeed = [0.0, 0.0]
  for f in features:
    if (eid, f) in weightByEidAndFeatureMap:
      weight_eid = weightByEidAndFeatureMap[(eid, f)]
    else:
      weight_eid = 0.0
    if (seed, f) in weightByEidAndFeatureMap:
      weight_seed = weightByEidAndFeatureMap[(seed, f)]
    else:
      weight_seed = 0.0
    # Jaccard similarity
    simWithSeed[0] += min(weight_eid, weight_seed)
    simWithSeed[1] += max(weight_eid, weight_seed)
  if simWithSeed[1] == 0:
    res = 0
  else:
    res = simWithSeed[0]*1.0/simWithSeed[1]
  if dpMap is not None:
    dpMap[key] = res
    return res, dpMap
  else:
    return res

def loadDocStrengthMap(filename):
    map = {}
    with open(filename, 'r') as fin:
        for line in fin:
            seg = line.strip('\r\n').split('\t')
            map[frozenset([int(seg[0]), int(seg[1])])] = float(seg[3])
    return map

def otsu(values):
  minV = 0.0
  rangeV = max(values)
  values = np.array(list(values))
  #print(minV, rangeV, values)
  values = (values-minV)/rangeV * 1000
  #print(values)
  hist = np.histogram(values, range(0, 1001))
  #print(hist)
  total = len(values)
  current_max, threshold = 0, 0
  sumT, sumF, sumB = 0, 0, 0
  k, ct = 0, 0
  for i in range(0, len(hist[0])):
    sumT += i * hist[0][i]
  weightB, weightF = 0, 0
  varBetween, meanB, meanF = 0, 0, 0
  for i in range(0, len(hist[0])):
    weightB += hist[0][i]
    weightF = total - weightB
    if weightF == 0:
      break
    #ct += hist[0][i]
    sumB += i*hist[0][i]
    sumF = sumT - sumB
    meanB = sumB/weightB
    meanF = sumF/weightF
    varBetween = weightB * weightF
    varBetween *= (meanB-meanF)*(meanB-meanF)
    if varBetween > current_max:
      current_max = varBetween
      threshold = i
      k = len(values) - ct
    ct += hist[0][i]

  #print("threshold is:", threshold, 'k is:', k)
  return k

#initialize
#seedEids = getSeedEids()
#seedEids = [int(sys.argv[2]), int(sys.argv[3])]
#orig_seeds = list(seedEids)
parent = getParent()
upperLevelEids = getUpperLevel()
data = sys.argv[1]
folder = '../../data/'+data+'/intermediate/'
print('loading eidToEntityMap...')
eidToEntityMap = loadEidToEntityMap(folder+'entity2id.txt') #entity2Eid.txt
threshold = 2 #tunable

print('loading skipgramsByEidMap, eidsBySkipgramMap...')
skipgramsByEidMap, eidsBySkipgramMap = loadFeaturesAndEidMap(folder+'eidSkipgramCounts.txt') #eidSkipgramCount.txt
print('loading weightByEidAndSkipgramMap...')
weightByEidAndSkipgramMap = loadWeightByEidAndFeatureMap(folder+'eidSkipgram2TFIDFStrength.txt') #(eid, feature, weight) file

print('loading typesByEidMap, eidsByTypeMap...')
typesByEidMap, eidsByTypeMap = loadFeaturesAndEidMap(folder+'eidTypeCounts.txt') #eidTypeCount.txt
print('loading weightByEidAndTypeMap...')
weightByEidAndTypeMap = loadWeightByEidAndFeatureMap(folder+'eidType2TFIDFStrength.txt') #(eid, feature, weight) file

print('loading eid pair doc strength map...')
docStrengthByEidPair = loadDocStrengthMap(folder+'eidPairDocStrength.txt')

seedEidPool = []
with open(sys.argv[2], 'r') as fseeds:
    for line in fseeds:
        seedEidPool.append(int(line.strip('\r\n').split('\t')[1]))

for i in range(0, len(seedEidPool)-2):
  seedEids = list(seedEidPool[i:i+3])
  orig_seeds = list(seedEids)
  dpMap_sg = {}
  dpMap_type = {}
  iters = 0
  print('==============================')
  print('==============================')
  print('Seeds:')
  for e in seedEids:
    print(eidToEntityMap[e], e)

  while iters < 30:
    #print('-----------------', iters, '------------------------')
    iters += 1
    prev_seeds = set(seedEids)
    #generate combined weight maps
    combinedWeightBySkipgramMap, numOfMatchesBySkipgramMap = getCombinedWeightByFeatureMapAndNumOfMatchesByFeatureMap(seedEids, skipgramsByEidMap, weightByEidAndSkipgramMap)
    #NOTE: loading type information
    combinedWeightByTypeMap, numOfMatchesByTypeMap = getCombinedWeightByFeatureMapAndNumOfMatchesByFeatureMap(seedEids, typesByEidMap, weightByEidAndTypeMap)

    nOfSeedEids = len(seedEids)

    #pruning pattern features
    redundantSkipgrams = set()
    for i in combinedWeightBySkipgramMap:
      size = len(eidsBySkipgramMap[i])
      # NOTE: filter those skipgram which matches over 30 or less than 3 entities
      if size < 3 or size > 30: 
        redundantSkipgrams.add(i)
    for sg in redundantSkipgrams:
      del combinedWeightBySkipgramMap[sg]

    #get final pattern features
    coreSkipgrams = []
    #print(len(combinedWeightBySkipgramMap))
    k = 100
    count = 0
    for sg in sorted(combinedWeightBySkipgramMap, key=combinedWeightBySkipgramMap.__getitem__, reverse=True):
      if count >= k:
        break
      count += 1
      if combinedWeightBySkipgramMap[sg]*1.0/nOfSeedEids > threshold:
        coreSkipgrams.append(sg)
    #print(len(coreSkipgrams))

    # NOTE: use type information
    coreTypes = []
    #print(len(combinedWeightByTypeMap))
    k = 10
    count = 0
    for ty in sorted(combinedWeightByTypeMap, key=combinedWeightByTypeMap.__getitem__, reverse=True):
        if count >= k:
            break
        count += 1
        if combinedWeightByTypeMap[ty]*1.0/nOfSeedEids > 0:
            coreTypes.append(ty)
    #print(len(coreTypes))

    #for sg in coreSkipgrams:
    #  print(sg)

    # NOTE: Terminate condition
    if len(coreSkipgrams) == 0:
      break

    combinedSgSimByCandidateEid = {}
    combinedTypeSimByCandidateEid = {}
    combinedSimByCandidateEid = {}
    candidates = set()

    # NOTE: the entity matched by core patterns are candidates, not the entity matched by core type
    # TODO: improvement maybe, consider the number of quality patterns matched by the candidate entities
    for sg in coreSkipgrams:
      candidates = candidates.union(eidsBySkipgramMap[sg])

    for eid in candidates:
      combinedSgSimByCandidateEid[eid] = 0.0
      combinedTypeSimByCandidateEid[eid] = 0.0
      #combinedSimByCandidateEid[eid] = 0.0
      for seed in seedEids:
        combinedSgSimByCandidateEid[eid] += getFeatureSim(eid, seed, weightByEidAndSkipgramMap, coreSkipgrams)
        #print(getFeatureSim(eid, seed, weightByEidAndTypeMap, eidsByTypeMap.keys(), dpMap=dpMap_type))
        combinedTypeSimByCandidateEid[eid] += 1.0#getFeatureSim(eid, seed, weightByEidAndTypeMap, coreTypes)
        #print(combinedSgSimByCandidateEid[eid])
        #print(increment)
        #combinedTypeSimByCandidateEid[eid] += increment

      ##SHOULD WE COMBINE THIS WAY? MULTIPLICATION SHOULD HAPPEN FOR EACH SEED OR FOR COMBINED VALUE???
      combinedSimByCandidateEid[eid] = combinedSgSimByCandidateEid[eid]*combinedTypeSimByCandidateEid[eid]*1.0/nOfSeedEids

    #get top k candidates for seed set of next iteration
    #k = otsu(combinedSimByCandidateEid.values())
    #print(k)
    k1 = 20
    k2 = 5
    count1, count2 = 0, 0
    #print('candidates: ', len(combinedSimByCandidateEid))
    for eid in sorted(combinedSimByCandidateEid, key=combinedSimByCandidateEid.__getitem__, reverse=True):
      if count1 >= k1 or count2 >= k2:
        break
        #if eid == 644125:
        #    print('Yeah!!! We reached United States!!', combinedSimByCandidateEid[eid])
      if eid not in seedEids:
        count1 += 1
        add = True
        if len(upperLevelEids) > 0:
          if frozenset([eid, parent]) in docStrengthByEidPair:
            simWithParent = docStrengthByEidPair[frozenset([eid, parent])]
          else:
            continue
          for p in upperLevelEids:
            if p == parent or frozenset([eid, p]) not in docStrengthByEidPair:
              continue
            simWithP = docStrengthByEidPair[frozenset([eid, p])]
            if simWithParent < simWithP:
              #print(eidToEntityMap[eid], simWithParent, simWithP, eidToEntityMap[p])
              add = False
              break
        if add:
          count2 += 1
          seedEids.append(eid)
          #print(eidToEntityMap[eid], combinedWeightByCandidateEid[eid]/len(coreSkipgrams))

          #if count == k:
          #    print(combinedSimByCandidateEid[eid])
    #if 644125 not in seedEids:
    #  seedEids.append(644125)
    #  print(eidToEntityMap[644125], combinedSimByCandidateEid[644125], combinedSgSimByCandidateEid[644125], combinedTypeSimByCandidateEid[644125])

    #coherence based pruning
    newSize = len(seedEids)
    percThreshold = 0.4 #tunable
    totalSim = {}
    for e1 in seedEids:
      sgSim = 0.0
      typeSim = 0.0
      totalSim[e1] = 0.0
      for e2 in seedEids:
        if e1 != e2:
          increment = getFeatureSim(e1, e2, weightByEidAndSkipgramMap, coreSkipgrams)
          sgSim += increment
          increment = getFeatureSim(e1, e2, weightByEidAndTypeMap, coreTypes)
          typeSim += increment
      totalSim[e1] = sgSim*1.0 # tunable?
      totalSim[e1] = 0.5*totalSim[e1]/(newSize-1) + 0.5*combinedSimByCandidateEid[eid]
      #if e1 == 644125 or e1 == 132050:
      #    print(e1, typeSim, sgSim, totalSim[e1])
    #k = newSize - otsu(totalSim.values())
    #count = 0
    flag = True
    maxsim = 0.0
    avesim = mean(list(totalSim.values()))
    st = pstdev(list(totalSim.values()))
    #print(avesim, st, avesim-st)
    for e in sorted(totalSim, key=totalSim.__getitem__, reverse=True):
      #if count >= k:
      #  break
      #count += 1
      if flag:
          maxsim = totalSim[e]
          flag = False
      if totalSim[e] < (avesim*1.0 - st*1.5):#percThreshold*maxsim:
          seedEids.remove(e)
          #print(eidToEntityMap[e], totalSim[e])
          continue
      #print(eidToEntityMap[e], e, totalSim[e])
      prev = totalSim[e]

    for s in orig_seeds:
      if s not in seedEids:
        seedEids.append(s)

    #for e in seedEids:
    #  print(eidToEntityMap[e], e, totalSim[e])

    if len(set(seedEids).difference(prev_seeds)) == 0 and len(prev_seeds.difference(set(seedEids))) == 0:
      break

    #parent similarity based pruning
    """
    if len(upperLevelEids) == 0:
        for e in seedEids:
          print(eidToEntityMap[e], e, totalSim[e])
        #print(eidToEntityMap[644125], totalSim[644125])
        continue
    toRemove = []
    for seed in seedEids:
      if frozenset([seed, parent]) in docStrengthByEidPair:
          simWithParent = docStrengthByEidPair[frozenset([seed, parent])]
      else:
          simWithParent = 0
          toRemove.append(seed)
          continue
      for p in upperLevelEids:
        if p == parent or frozenset([seed, p]) not in docStrengthByEidPair:
          continue
        simWithP = docStrengthByEidPair[frozenset([seed, p])]
        if simWithParent < simWithP:
          print(eidToEntityMap[seed], simWithParent, simWithP, eidToEntityMap[p])
          toRemove.append(seed)
          break
    for e in toRemove:
      seedEids.remove(e)
    """

    if len(set(seedEids).difference(prev_seeds)) == 0 and len(prev_seeds.difference(set(seedEids))) == 0:
      break
  print('-----------------------')
  print('Expanded set:')
  for e in seedEids:
    print(eidToEntityMap[e], e)