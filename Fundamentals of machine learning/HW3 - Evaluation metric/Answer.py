import numpy as np

def Accuracy(label, pred):
    ########################################################################################
    # TODO : Complete the code to calculate the accuracy for prediction.
    #         [Input]
    #         - label : (N, ), Correct label with 0 (negative) or 1 (positive)
    #         - hypo  : (N, ), Predicted score between 0 and 1
    #         [output]
    #         - Acc : (scalar, float), Computed accuracy score
    # ========================= EDIT HERE =========================
    Acc = None
    TN = 0
    FN = 0
    TP = 0
    FP = 0
    for i in range(len(label)):
        if label[i] == pred[i] and label[i] == 1:
            TP += 1
        elif label[i] == pred[i] and label[i] == 0:
            TN += 1
        elif label[i] == 1:
            FN += 1
        else:
            FP += 1
    Acc = (TP + TN) / (TP + FP + FN + TN)
    # =============================================================
    return Acc

def Precision(label, pred):
    ########################################################################################
    # TODO : Complete the code to calculate the Precision for prediction.
    #         you should consider that label = 1 is positive. 0 is negative
    #         Notice that, if you encounter the divide zero, return 1
    #         [Input]
    #         - label : (N, ), Correct label with 0 (negative) or 1 (positive)
    #         - hypo  : (N, ), Predicted score between 0 and 1
    #         [output]
    #         - precision : (scalar, float), Computed precision score
    # ========================= EDIT HERE =========================
    precision = None
    TN = 0
    FN = 0
    TP = 0
    FP = 0
    for i in range(len(label)):
        if label[i] == pred[i] and label[i] == 1:
            TP += 1
        elif label[i] == pred[i] and label[i] == 0:
            TN += 1
        elif label[i] == 1:
            FN += 1
        else:
            FP += 1
    if TP + FP == 0:
        return 1
    precision = TP / (TP + FP)
    # =============================================================
    return precision

def Recall(label, pred):
    ########################################################################################
    # TODO : Complete the code to calculate the Recall for prediction.
    #         you should consider that label = 1 is positive. 0 is negative
    #         Notice that, if you encounter the divide zero, return 1
    #         [Input]
    #         - label : (N, ), Correct label with 0 (negative) or 1 (positive)
    #         - hypo  : (N, ), Predicted score between 0 and 1
    #         [output]
    #         - recall : (scalar, float), Computed recall score
    # ========================= EDIT HERE =========================
    recall = None
    TN = 0
    FN = 0
    TP = 0
    FP = 0
    for i in range(len(label)):
        if label[i] == pred[i] and label[i] == 1:
            TP += 1
        elif label[i] == pred[i] and label[i] == 0:
            TN += 1
        elif label[i] == 1:
            FN += 1
        else:
            FP += 1
    if TP + FN == 0:
        return 1
    recall = TP / (TP + FN)
    # =============================================================
    return recall

def F_measure(label, pred):
    ########################################################################################
    # TODO : Complete the code to calculate the F-measure score for prediction.
    #         you can erase the code. (F_score = 0.)
    #         Notice that, if you encounter the divide zero, return 1
    #         [Input]
    #         - label : (N, ), Correct label with 0 (negative) or 1 (positive)
    #         - hypo  : (N, ), Predicted score between 0 and 1
    #         [output]
    #         - F_score : (scalar, float), Computed F-score score
    # ========================= EDIT HERE =========================
    F_score = None
    P = Precision(label, pred)
    R = Recall(label, pred)
    if P + R == 0 :
        return 1
    F_score = 2 * P * R / (P + R)
    # =============================================================
    return F_score

def MAP(label, hypo, at = 10):
    ########################################################################################
    # TODO : Complete the code to calculate the MAP for prediction.
    #         Notice that, hypo is the real value array in (0, 1)
    #         MAP (at = 10) means MAP @10
    #         [Input]
    #         - label : (N, K), Correct label with 0 (incorrect) or 1 (correct)
    #         - hypo  : (N, K), Predicted score between 0 and 1
    #         - at: (int), # of element to consider from the first. (TOP-@)
    #         [output]
    #         - Map : (scalar, float), Computed MAP score
    # ========================= EDIT HERE =========================
    Map = None
    AP = []
    for i in range(hypo.shape[0]):
        list = []
        relevant = 0
        for j in range(hypo.shape[1]):
            list.append((label[i][j], hypo[i][j]))
            if label[i][j] == 1:
                relevant += 1
        ap = 0
        cnt = 0
        list.sort(key=lambda x: x[1], reverse=True)
        for i in range(at):
            if list[i][0] == 1:
                cnt += 1
            ap += list[i][0] * (cnt / (i + 1))
        ap /= relevant
        AP.append(ap)
    Map = np.mean(AP)
    # =============================================================
    return Map

def nDCG(label, hypo, at = 10):
    ########################################################################################
    # TODO : Complete the each code to calculate the nDCG for prediction.
    #         you can erase the code. (dcg, idcg, ndcg = 0.)
    #         Notice that, hypo is the real value array in (0, 1)
    #         nDCG (at = 10 ) means nDCG @10
    #         [Input]
    #         - label : (N, K), Correct label with 0 (incorrect) or 1 (correct)
    #         - hypo  : (N, K), Predicted score between 0 and 1
    #         - at: (int), # of element to consider from the first. (TOP-@)
    #         [output]
    #         - Map : (scalar, float), Computed nDCG score

    def DCG(label, hypo, at=10):
        # ========================= EDIT HERE =========================
        dcg = None
        Dcg = []
        for i in range(hypo.shape[0]):
            list = []
            for j in range(hypo.shape[1]):
                list.append((label[i][j], hypo[i][j]))
            list.sort(key=lambda x: x[1], reverse=True)
            _dcg = 0
            for j in range(at):
                _dcg += list[j][0] / np.log2(j+2)
            Dcg.append(_dcg)
        dcg = Dcg
        # =============================================================
        return dcg

    def IDCG(label, hypo, at=10):
        # ========================= EDIT HERE =========================
        idcg = None
        Idcg = []
        for i in range(hypo.shape[0]):
            _idcg = 0
            list = []
            for j in range(hypo.shape[1]):
                list.append((label[i][j], hypo[i][j]))
            list.sort(key=lambda x: x[0], reverse=True)
            for j in range(at):
                    _idcg += list[j][0] / np.log2(j+2)
            Idcg.append(_idcg)
        idcg = Idcg
        # =============================================================
        return idcg
    # ========================= EDIT HERE =========================
    dcg = DCG(label, hypo, at)
    idcg = IDCG(label, hypo, at)
    list = [dcg[x]/idcg[x] for x in range(len(dcg))]
    ndcg = np.mean(list)
    # =============================================================
    return ndcg

# =============================================================== #
# ===================== DO NOT EDIT BELOW ======================= #
# =============================================================== #

def evaluation_test1(label, pred, at = 10):
    result = {}

    result['Accuracy '] = Accuracy(label, pred)
    result['Precision'] = Precision(label, pred)
    result['Recall   '] = Recall(label, pred)
    result['F_measure'] = F_measure(label, pred)

    return result

def evaluation_test2(label, hypo, at = 10):
    result = {}

    result['MAP  @%d'%at] = MAP(label, hypo, at)
    result['nDCG @%d'%at] = nDCG(label, hypo, at)

    return result
