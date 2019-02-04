
(function () {

    var State = {
        PENDING: 0,
        FULFILLED: 1,
        REJECTED: 2
    };

    Promise = function (executor) {
        this.state = State.PENDING;
        this.value = undefined;

        var p = this;

        try {
            executor(function (result) {
                setValue(p, result);
            }, function (reason) {
                setReason(p, reason);
            });
        } catch (reason) {
            setReason(p, reason);
        }

    };

    Promise.reject = function (reason) {
        var v = Object.create(Promise.prototype, {});
        v.state = State.REJECTED;
        v.value = reason;
        return v;
    };

    Promise.resolve = function (value) {
        var v = Object.create(Promise.prototype, {});
        v.state = State.FULFILLED;
        v.value = value;
        return v;
    };

    Promise.all = function (items) {
        return new Promise(function (resolve, reject) {
            var count = 0;
            var maxCount = items.length;
            var done = false;
            var values = [];

            for (var i = 0; i < items.length; i++) {

                (function (i, item) {

                    item.then(function (value) {

                        if (done) {
                            return;
                        }

                        values[i] = value;
                        count++;
                        if (count == maxCount) {
                            done = true;
                            resolve(values);
                        }
                    }, function (reason) {

                        if (done) {
                            return;
                        }

                        done = true;

                        reject(reason);
                    });

                })(i, items[i]);
            }
        });
    };

    Promise.race = function (items) {
        return new Promise(function (resolve, reject) {
            var done = false;
            for (var i = 0; i < items.length; i++) {
                var item = items[i];
                item.then(function (value) {

                    if (done) {
                        return;
                    }
                    done = true;
                    resolve(value);
                }, function (reason) {

                    if (done) {
                        return;
                    }

                    done = true;
                    reject(reason);
                });
            }
        });
    };

    function setValue(p, value) {
        if (p.state != State.PENDING) {
            throw "Promise not is PENDING";
        }

        if (value instanceof Promise) {
            value.then(function (value) {
                try {
                    setValue(p, value);
                } catch (reason) {
                    setReason(p, reason);
                }
            }, function (reason) {
                setReason(p, reason);
            });
        }

        if (typeof p._onResolve == 'function') {

            var fn = p._onResolve;

            p._onResolve = undefined;

            try {
                var newValue = fn(value);
                if (newValue === undefined) {
                    newValue = value;
                }
                setValue(p, newValue);
            } catch (reason) {
                setReason(p, reason);
            }

        } else {
            p.state = State.FULFILLED;
            p.value = value;
        }
    }

    function setReason(p, reason) {

        if (p.state != State.PENDING) {
            throw "Promise not is PENDING";
        }

        if (typeof p._onReject == 'function') {

            var fn = p._onReject;

            p._onReject = undefined;

            try {
                fn(reason);
                setReason(p, reason);
                p.state = State.REJECTED;
                p.value = reason;
            } catch (reason) {
                setReason(p, reason);
            }

        } else {
            p.state = State.REJECTED;
            p.value = reason;
        }

    }

    Promise.prototype = Object.create(Object.prototype, {
        then: {
            value: function (onResolve, onReject) {

                var v = Object.create(Promise.prototype, {});
                v.state = State.PENDING;
                this._onResolve = function (value) {
                    if (typeof onResolve == 'function') {
                        try {
                            var newValue = onResolve(value);
                            if (newValue === undefined) {
                                newValue = value;
                            }
                            setValue(v, newValue);
                        } catch (reason) {
                            setReason(v, reason);
                        }
                    } else {
                        try {
                            setValue(v, value);
                        } catch (reason) {
                            setReason(v, reason);
                        }
                    }
                };
                this._onReject = function (reason) {
                    if (typeof onReject == 'function') {
                        try {
                            onReject(reason);
                            setReason(v, reason);
                        } catch (r) {
                            setReason(v, r);
                        }
                    } else {
                        setReason(v, reason);
                    }
                };
                if (this.state == State.FULFILLED) {
                    this._onResolve(this.value);
                } else if (this.state == State.REJECTED) {
                    this._onReject(this.value);
                }
                return v;

            },
            writable: false,
            configurable: false,
            enumerable: true
        },
        catch: {
            value: function (onReject) {
                return this.then(undefined, onReject);
            },
            writable: false,
            configurable: false,
            enumerable: true
        },
        finally: {
            value: function (onFinally) {
                var fn = function () {
                    if (typeof onFinally == 'function') {
                        try {
                            onFinally();
                        } catch (e) {

                        }
                    }
                };
                return this.then(fn, fn);
            },
            writable: false,
            configurable: false,
            enumerable: true
        }
    });

})();
